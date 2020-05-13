// Standard dimensions

STANDARD_DEPTH = 4;


// Chassis dimensions

/* AFTER DESIGN HAS BEEN IMPLEMENTED, ONLY THESE VALUES SHOULD BE ADJUSTED */

motor_drop = -4;

motor_radius = 12.5;
motor_depth = 35;

motor_base_radius = motor_radius + STANDARD_DEPTH;
motor_base_depth = motor_depth + STANDARD_DEPTH;

chassis_x = motor_base_radius*2 + 115;
chassis_y = 90;
chassis_z = 5;

tread_length = 120;
tread_width = 15;

sensor_length = 7.5;
sensor_width = 15;

arm_length = tread_width + 2*sensor_length;
arm_width = sensor_width + (2*STANDARD_DEPTH);

bearing_radius = 3;



// Main chassis structure


difference() {
    difference() {
        union() {
            union() {
                // Chassis bed
                cube([chassis_x, chassis_y, chassis_z]);
                
                // Left motor base housing
                translate([motor_base_radius, 0, motor_drop]) {
                    rotate([270, 0, 0]) {
                        cylinder(motor_base_depth, motor_base_radius, motor_base_radius, false);
                    }
                }
            }
            
            // Right motor base housing
            translate([motor_base_radius, chassis_y, motor_drop]) {
                rotate([90, 0, 0]) {
                    cylinder(motor_base_depth, motor_base_radius, motor_base_radius, false);
                }
            }
        }
        
        // Left motor cutout
        translate([motor_base_radius, -1, motor_drop]) {
            rotate([270, 0, 0]) {
                cylinder(motor_depth, motor_radius, motor_radius, false);
            }
        }
    }
    
    // Right motor cutout
    translate([motor_base_radius, chassis_y + 1, motor_drop]) {
        rotate([90, 0, 0]) {
            cylinder(motor_depth, motor_radius, motor_radius, false);
        }
    }
}

// Left bearing
translate([motor_base_radius+tread_length, 0, motor_drop]) {
    rotate([270, 0, 0]) {
        difference() {
            cylinder(STANDARD_DEPTH*3, STANDARD_DEPTH+bearing_radius, STANDARD_DEPTH+bearing_radius, false);
            translate([0, 0, -0.5]) {
                cylinder(STANDARD_DEPTH*3+1, bearing_radius, bearing_radius, false);
            }
        }
    }
}

// Right bearing
translate([motor_base_radius+tread_length, chassis_y, motor_drop]) {
    rotate([90, 0, 0]) {
        difference() {
            cylinder(STANDARD_DEPTH*3, STANDARD_DEPTH+bearing_radius, STANDARD_DEPTH+bearing_radius, false);
            translate([0, 0, -0.5]) {
                cylinder(STANDARD_DEPTH*3+1, bearing_radius, bearing_radius, false);
            }
        }
    }
}

// Arm sensor holder
translate([(chassis_x - arm_width)/2, 0, 0]) {
    rotate([0, 0, 270]) {
        difference() {
            cube([arm_length, arm_width, chassis_z]);
            translate([arm_length - sensor_length - STANDARD_DEPTH, (arm_width-sensor_width)/2, -0.5]) {
                cube([sensor_length, sensor_width, chassis_z+1]);
            }
        }

        // Frontmost support
        polyhedron(
        points = 
        [[arm_length/2, 0, 0], //0
        [arm_length/2, 0, chassis_z], //1 
        [0, 0, 0], //2
        [0, 0, chassis_z], //3
        [0, -(2*STANDARD_DEPTH), 0], //4
        [0, -(2*STANDARD_DEPTH), chassis_z]], //5
        faces = 
        [[0, 2, 4], // bottom
        [1, 3, 5], // top
        [4, 5, 1, 0], // exposed face
        [0, 1, 3, 2], // arm face
        [3, 2, 4, 5]] // chassis face
        );
        
        // Rearmost support
        polyhedron(
        points = 
        [[arm_length/2, arm_width, 0], //0
        [arm_length/2, arm_width, chassis_z], //1 
        [0, arm_width, 0], //2
        [0, arm_width, chassis_z], //3
        [0, arm_width + (2*STANDARD_DEPTH), 0], //4
        [0, arm_width + (2*STANDARD_DEPTH), chassis_z]], //5
        faces = 
        [[0, 2, 4], // bottom
        [1, 3, 5], // top
        [4, 5, 1, 0], // exposed face
        [0, 1, 3, 2], // arm face
        [3, 2, 4, 5]] // chassis face
        );
    }
}

// Front left sensor holder
translate([0, sensor_width + 2*STANDARD_DEPTH, 0]) {
    rotate([0, 0, 180]) {
        difference() {
            cube([sensor_length + STANDARD_DEPTH, sensor_width + 2*STANDARD_DEPTH, chassis_z]);
            translate([0, STANDARD_DEPTH, -0.5]) {
                cube([sensor_length, sensor_width, chassis_z+1]);
            }
        }
    }
}

// Front right sensor holder
translate([0, chassis_y, 0]) {
    rotate([0, 0, 180]) {
        difference() {
            cube([sensor_length + STANDARD_DEPTH, sensor_width + 2*STANDARD_DEPTH, chassis_z]);
            translate([0, STANDARD_DEPTH, -0.5]) {
                cube([sensor_length, sensor_width, chassis_z+1]);
            }
        }
    }
}


// H bridge bed
translate([0, sensor_width+(STANDARD_DEPTH*2), 0]) {
    cube([STANDARD_DEPTH, chassis_y - 2*(sensor_width+(STANDARD_DEPTH*2)), motor_base_radius+motor_drop]);
    translate([0, 0, motor_base_radius+motor_drop-3]) {
        cube([motor_base_radius*2, chassis_y - 2*(sensor_width+(STANDARD_DEPTH*2)), STANDARD_DEPTH]);
    }
}