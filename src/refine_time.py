#!/usr/bin/env python
import rosbag
import rospy

flag1 = True
timebase1 = -1
timebase2 = -1
flag2 = True

with rosbag.Bag('/home/tommy/bagfile/tf_merged.bag', 'w') as outbag:
	for topic, msg, t in rosbag.Bag('/home/tommy/bagfile/api_output_test2.bag').read_messages():

		if msg._has_header:
			if topic == "/mavros/imu/data0" and flag1 == True:
				timebase1 = msg.header.stamp
				flag1 = False

			if topic == "/mavros/imu/data1" and flag2 == True:
				timebase2 = msg.header.stamp
				flag2 = False

			newTime = msg.header.stamp

			if topic[-1] != 0 and timebase1 != -1 and timebase2 != -1:
				newTime = msg.header.stamp - (timebase2 - timebase1)
				#print(msg.header.stamp,newTime)

			outbag.write(topic, msg, newTime)

		#print("timestamp refinement complete!")

