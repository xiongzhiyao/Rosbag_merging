#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <sensor_msgs/Imu.h>
#include <boost/foreach.hpp>
#include "ros/ros.h"
#define foreach BOOST_FOREACH

class mergeBag {
	std::vector<sensor_msgs::Imu::ConstPtr> Imu_1_data;
	std::vector<sensor_msgs::Imu::ConstPtr> Imu_2_data;
public:
	mergeBag(std::string bagname_r1, std::string bagname_r2, std::vector<std::string> topics){

		rosbag::Bag bag_read1;
		bag_read1.open(bagname_r1, rosbag::bagmode::Read);
		
		rosbag::View view1(bag_read1, rosbag::TopicQuery(topics));

		foreach(rosbag::MessageInstance const m, view1){
			
			std::cout<<m.getTopic()<<std::endl;
			sensor_msgs::Imu::ConstPtr s = m.instantiate<sensor_msgs::Imu>();

			if (s != NULL){
				
				std::cout << s->header.stamp <<" || "<<s->orientation.x << std::endl;	
				Imu_1_data.push_back(s);
			}	
		}
		bag_read1.close();
		
		rosbag::Bag bag_read2;
		bag_read2.open(bagname_r2, rosbag::bagmode::Read);
		
		rosbag::View view2(bag_read2, rosbag::TopicQuery(topics));
		foreach(rosbag::MessageInstance const m, view2){
			
			std::cout<<m.getTopic()<<std::endl;
			std::cout<<m.getDataType()<<std::endl;
			sensor_msgs::Imu::ConstPtr s = m.instantiate<sensor_msgs::Imu>();
			if (s != NULL){

				std::cout << s->header.stamp <<" || "<<s->orientation.x << std::endl;	
				std::cout << s->orientation.x << std::endl;	
				Imu_2_data.push_back(s);			
			}	
		}

		rosbag::View view(bag_read2);
		std::vector<const rosbag::ConnectionInfo *> connection_infos = view.getConnections();

		BOOST_FOREACH(const rosbag::ConnectionInfo *info, connection_infos) {
			std::cout<<info->topic<<std::endl;
			std::cout<<info->datatype<<std::endl;
		}

		bag_read2.close();

	}
	void output_bag(std::string bagname_w){

		rosbag::Bag bag_write;
		bag_write.open(bagname_w, rosbag::bagmode::Write);
		double sec_diff = time_diff(Imu_1_data[0],Imu_2_data[0]);
		ros::Duration time_offset(sec_diff);

		for (int i = 0; i < std::min(Imu_1_data.size(),Imu_2_data.size()); i++){
			sensor_msgs::Imu::ConstPtr s1 = Imu_1_data[i];
			sensor_msgs::Imu::ConstPtr s2 = Imu_2_data[i];
			
			bag_write.write("rewrote_imu_1", s1->header.stamp, Imu_1_data[i]);
			bag_write.write("rewrote_imu_2", s2->header.stamp + time_offset, Imu_2_data[i]);
		}
		bag_write.close();
	}
	double time_diff(sensor_msgs::Imu::ConstPtr s1, sensor_msgs::Imu::ConstPtr s2){
		return s1->header.stamp.toSec() - s2->header.stamp.toSec();
	}
};

int main(int argc, char **argv){
	ros::init(argc, argv, "api_test");
	ros::Time::init();
	const char* args[] = {"/mavros/imu/data"};
	std::vector<std::string> topic(args, args + sizeof(args)/sizeof(args[0]));
	mergeBag merge("/home/tommy/bagfile/2018-03-21-23-12-18.bag","/home/tommy/bagfile/2018-03-21-23-13-12.bag",topic);
	merge.output_bag("/home/tommy/bagfile/api_output_test.bag");
}
