# LegionATT

PROJECT DOCUMENTATION IS NOT YET COMPLETE

Star Wars Legion ATT Motorized Modification

This project includes basic information, items, and files i used to motorize an ATT from the game Star Wars Legion. The files include the code needed for the Adafuit RF52 Bluefruit LE, Raspberry PI W python script and STL files for mounting the motors, turret and side guns.

The idea of the project is to use a Raspberry PI Zero W as a controller to move the turret and side guns of a Star Wars Legion ATT. The ATT's side guns and turret are connected to low speed motors which are controlled by an Adafruit Feather RF52 Bluefruit LE. Connectivity between the PI and Feather used Bluetooth.

To complete the final build you will need the following or similar items:
   Star Wars Legion ATT miniture
   Two N20 low speed motors
   Toy car gear and axle kit with various sizes of axles and belt pulley gears.
   Adafruit RF52 Bluefruit LE - RF52832
   Raspberry PI Zero W
   Four 3mm x 7mm x 3mm ball bearings (for the side guns)
   One 12mm x 18mm x 4mm ball bearing for the turret)
   
The STL files are used to mount the motors, side guns and turret
  attTurret.stl - STL file for parts needed to mount the main turret to the body using a bearing for smooth movement.
  ATTTurretMotorMount.stl - STL file for parts needed to mount the N20 DC motor inside the body and drive the turret.
  ATTGunBearing.stl - STL file for parts needed to mount bearings and axel to the side guns.
  
Code for the Adafruit RF52 Bluefruit LE

Python script for the Raspberry PI Zero W
