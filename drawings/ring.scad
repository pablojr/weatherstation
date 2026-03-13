module ring(inner_d, outer_d, height) {
    inner_r = inner_d / 2;
    outer_r = outer_d / 2;
    linear_extrude(height)
        difference() {
            circle(outer_r);
            circle(inner_r);
        }
}

ring(inner_d=6.2, outer_d=7.8, height=5);