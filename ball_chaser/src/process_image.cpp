#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <ros/console.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_joint");
    
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    // Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    bool ball_present = false;
    int pos = 0;
    int j = 0;

    // Loop through each pixel in the image and check if its equal to the white pixel
    for (int i = 0; i < img.height*img.width; i++) {
		if (img.data[j] == white_pixel && img.data[j+1] == white_pixel && img.data[j+2] == white_pixel) {
		    ball_present = true;
                    pos=(j%img.step)/(img.step/img.width);
		    break;
		} 
		else
		    j+=3;  
    }

    // If ball is present then move the robot to the ball depending on where it lies
    if (ball_present == true){
       // Turn the robot left
       if(pos<img.width/3)
            drive_robot(0,0.1);
       // Move the robot forward
       else if(pos>= img.width/3&& pos<=2*img.width/3)
	    drive_robot(0.5,0);
       // Turn the robot right
       else
            drive_robot(0,-0.1);
    }
    else
       // Stop the robot
            drive_robot(0,0);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}

