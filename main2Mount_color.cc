// Unclean version
ColorGrid *getImage(int *elev_data, int width, int height, int maxVal) {
    // create color grid
    ColorGrid *cg = new ColorGrid(height, width);

    float pixel_val;
    int n = 0, gray, blue, green, LYellow;

    // load the elevation data
    for (int j = 0; j < height; j++)
    for (int k = 0; k < width; k++) {
        pixel_val = (float) elev_data[n++];
        if (pixel_val == -1.) {
            // this is the path drawn in purple(255, 0, 255) across the image Red(255, 8, 0)
            cg->set(j, k, Color(255, 8, 0));
            continue;
        }

        if(maxVal == 0) maxVal = 1;
        float scale = pixel_val/maxVal;
        //within 0-0.24 blue
        if(scale < 0.25){
        //divide by 0.25 to get within 0-1 scale, based on 1 times by 127(to not start with black) because 127+128 = 255(max) and 128(min)
            blue = 128 + (int) (scale/0.25 * 127);
            cg->set(j, k, Color(0, 0, blue));
        }
        //0.25-0.5 green
        else if(scale < 0.5){
        //Subtract minimum(0.25) so that it could start it at a lower range,
        //so that when divided by 0.25 to get within the range of 0-1 the
        //multipier doesn't always give such a big value to smaller numbers(like 0.27)
        //This also means green can't hit the max of 255.
            //green = (int) ((scale-0.25)/0.25 * 255);//original
            //green = 50 + (int) ((scale-0.25)/0.25 * 150);//Pair
            //green = 70 + (int) ((scale-0.25)/0.25 * 100);
            //green = 40 + (int) ((scale-0.25)/0.25 * 100);// stick
            green =10 + (int) ((scale-0.25)/0.25 * 100);
            //int greenMax = green - 10;
            int finalGreen = green -30;
            if (finalGreen < 0) finalGreen = 0;
            if (finalGreen > 255) finalGreen = 255;
            //if(greenMax < 0) greenMax = 0;
            //if(greenMax < 50) greenMax = greenMax+60;
            //if(greenMax < 90) greenMax = greenMax+30;
            //if(greenMax > 130) greenMax = greenMax-40;
            //if(greenMax > 200) greenMax = greenMax-50;
            //bright green
            //cg->set(j, k, Color(25, green/2, 53));
            //what I was playing with
            //cg->set(j, k, Color(74, greenMax, 53));
            cg->set(j, k, Color(finalGreen, green, 25));
        }
        //0.50-0.75 light yellow
        else if(scale < 0.75){
            LYellow = (int) ((scale-0.5)/0.5 * 255);
            int BYellow = LYellow+50;
            if(BYellow < 0) BYellow = 0;
            if(BYellow < 50) BYellow = BYellow+60;
            if(BYellow < 90) BYellow = BYellow+30;
            if(BYellow > 255) BYellow = 255;
            //if(greenMax > 130) greenMax = BYellow-20;
            //if(greenMax > 200) greenMax = BYellow-20;
            //cg->set(j, k, Color(LYellow, LYellow+25, 53)); //original try light green
            //cg->set(j, k, Color(LYellow + 12, LYellow, 44));//darker yellow
            //cg->set(j, k, Color(LYellow + 12, LYellow+20, 5));//darker yellow green
            cg->set(j, k, Color(BYellow,BYellow, 13));//other like
        }
        //0.75-1 white
        else{
            gray = (int) (pixel_val*255.0/maxVal);
            cg->set(j, k, Color(gray, gray, gray));
        }
    }
    return cg;
}

//clean version
ColorGrid *getImage(int *elev_data, int width, int height, int maxVal) {
    // create color grid
    ColorGrid *cg = new ColorGrid(height, width);

    float pixel_val;
    int n = 0, gray, blue, green, LYellow;

    // load the elevation data
    for (int j = 0; j < height; j++)
    for (int k = 0; k < width; k++) {
        pixel_val = (float) elev_data[n++];
        if (pixel_val == -1.) {
            // this is the path drawn in purple(255, 0, 255) across the image Red(255, 8, 0)
            cg->set(j, k, Color(255, 8, 0));
            continue;
        }

        if(maxVal == 0) maxVal = 1;
        float scale = pixel_val/maxVal;
        //within 0-0.24 blue
        if(scale < 0.25){
        //divide by 0.25 to get within 0-1 scale, based on 1 times by 127(to not start with black) because 127+128 = 255(max) and 128(min)
            blue = 128 + (int) (scale/0.25 * 127);
            cg->set(j, k, Color(0, 0, blue));
        }
        //0.25-0.5 green
        else if(scale < 0.5){
        //Subtract minimum(0.25) so that it could start it at a lower range,
        //so that when divided by 0.25 to get within the range of 0-1 the
        //multipier doesn't always give such a big value to smaller numbers(like 0.27)
        //This also means green can't hit the max of 255.
            green =10 + (int) ((scale-0.25)/0.25 * 100);
            int finalGreen = green -30;
            if (finalGreen < 0) finalGreen = 0;
            if (finalGreen > 255) finalGreen = 255;
            cg->set(j, k, Color(finalGreen, green, 25));
        }
        //0.50-0.75 light yellow
        else if(scale < 0.75){
            LYellow = (int) ((scale-0.5)/0.5 * 255);
            int BYellow = LYellow+50;
            if(BYellow < 0) BYellow = 0;
            if(BYellow < 50) BYellow = BYellow+60;
            if(BYellow < 90) BYellow = BYellow+30;
            if(BYellow > 255) BYellow = 255;
            cg->set(j, k, Color(BYellow,BYellow, 13));//other like
        }
        //0.75-1 white
        else{
            gray = (int) (pixel_val*255.0/maxVal);
            cg->set(j, k, Color(gray, gray, gray));
        }
    }
    return cg;
}
