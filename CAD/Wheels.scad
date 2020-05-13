
$fn=75;

module spokes(num, length, width, height) {
    j = num;
    
    intersection() {
        cylinder(height, length, length, True);
        for (i=[0:j]) {
            rotate([0, 0, (360/j)*i]) {
                translate([0, -(width/2), 0]) {
                    cube([length, width, height]);
                }
            }
        }
    }
}

module wheel(num, length, width, height) {
    union() {
        spokes(num, length, width, height);
        difference() {
            cylinder(height, length + (1.5*width), length + (1.5*width), True);
            translate([0, 0, -0.5]) {
                cylinder(height+1, length, length, True);
            }
        }
    }
}

difference() {
    wheel(12, 20, 3, 10);
    translate([0, 0, -1]) {
        cylinder(100, 1.1, 1.1, True);
    }
}


translate([100, 0, 0]) {
    difference() {
        wheel(12, 20, 3, 10);
        translate([0, 0, -1]) {
            cylinder(100, 3, 3, True);
        }
    }
}

translate([50, 0, 0]) {
    cylinder(35, 2.8, 2.8, True);
    cylinder(5, 6, 6, True);
}

translate([50, 15, 0]) {
    difference() {
        cylinder(5, 6, 6, True);
        cylinder(6, 3, 3, True);
    }
}