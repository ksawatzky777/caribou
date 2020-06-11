# -*- coding: utf-8 -*-
"""
This is a data formatting script to extract long-lat data from ERA5 GRIB1 and 
GRIB2 files from the climate data store and than convert the lat-long data to 
a cartesian coordinate system suitable for CARIBOU input.
"""

import pygrib
import numpy as np
import pandas as pd
import os
import sys

def convert_by_radius(file, p_lat, p_long, r, r_earth=6367.47):
    """
    This function is not complete.
    TODO: Complete this.
    """
    #Open the GRIB file.
    grbs = pygrib.open(file)
    grb_u = grbs.select(name='U component of wind')
    grb_v = grbs.select(name='V component of wind')
    grb_w = grbs.select(name='Vertical velocity')
    
    pass #Add the return later.

def convert_by_latlong(file, point1, point2, p_level, deltas=[31, 31]):
    """
    This function extracts long-lat data within a bounded region defined by 
    the input parameters. It than converts the long/lat coordinates to an 
    x-y-z cartesian grid where (0,0,0) is located at point1. The resulting 
    coordinate transformed data is output to three csv files formatted to 
    become initial conditions for CARIBOU executions.
    
    Input parameters:
        file: string of the filename.
        point1: python list where entry 0 is the latitude and entry 1 is the 
        longitude.
        point2: python list where entry 0 is the latitude and entry 1 is the 
        longitude.
        p_level: python list of isobaric pressure levels.
        delta: python list of coordinate transformation factors to convert 
        lat/long values to x (entry 0) and y (entry 1) values. By default, 
        this is 31 km between datapoints in both lat and long (default for 
        ERA5 reanalysis). 
        
    Outputs:
        u.csv (csv file containing the magnitude of the east-facing wind 
        component, m/s).
        v.csv (csv file containing the magnitude of the north-facing wind 
        component), m/s.
        w.csv (csv file containing the magnitude of the updraft velocity, 
        Pa/s).
        All output csv files also contain the x-y-z coordinates of each 
        datapoint in columns 1, 2 and 3. The remaining data is contained in 
        columns 1 -> n, where n is the number of pressure levels.
    """
    
    print('Converting data, this will take some time')
    
    #Declaring output directory
    output_dir = './'
    
    #Open the GRIB file.
    grbs = pygrib.open(file)
    
    #Designating GRIB messages at the initial pressure level (for fetching 
    #lats and longs).
    grb_u = grbs.select(name='U component of wind',
                        typeOfLevel='isobaricInhPa', level=p_level[0])[0]
    grb_v = grbs.select(name='V component of wind',
                        typeOfLevel='isobaricInhPa', level=p_level[0])[0]
    grb_w = grbs.select(name='Vertical velocity', 
                        typeOfLevel='isobaricInhPa', level=p_level[0])[0]
    
    #Fetch data from the GRIB file and assign it to a numpy array for the 
    #first pressure level.
    u, lats, longs = grb_u.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
    v, lats, longs = grb_v.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
    w, lats, longs = grb_w.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
    
    #Declaring numpy arrays for the transformed coordinate system.
    grid_x = np.ones_like(longs)
    grid_y = np.ones_like(lats)
    
    #Generating the 2-D cartesian grid from lats and longs.
    for i in range(np.shape(longs)[0]):
        for j in range(np.shape(longs)[1]):
            grid_x[i,j] = i*(deltas[0])
    
    for i in range(np.shape(lats)[0]):
        for j in range(np.shape(lats)[1]):
            i_ = (np.shape(lats)[0] - i - 1)
            j_ = (np.shape(lats)[1] - j - 1)
            grid_y[i_,j_] = (j_)*(deltas[1])
    
    #Flatten both x and y grid components into 1-D arrays
    flat_x = grid_x.flatten()
    flat_y = grid_y.flatten()
    
    #Building initial dictionaries for the dataframes.
    coords = {'x': flat_x, 'y': flat_y}
    
    #Building 3 pandas dataframes to store the data.
    df_u = pd.DataFrame(data=coords)
    df_v = pd.DataFrame(data=coords)
    df_w = pd.DataFrame(data=coords)
    
    #Loop through each pressure level.
    for i in range(len(p_level)):
        #Designate GRIB messages for each velocity at each pressure level.
        grb_u = grbs.select(name='U component of wind',
                            typeOfLevel='isobaricInhPa', level=p_level[i])[0]
        grb_v = grbs.select(name='V component of wind',
                            typeOfLevel='isobaricInhPa', level=p_level[i])[0]
        grb_w = grbs.select(name='Vertical velocity', 
                            typeOfLevel='isobaricInhPa', level=p_level[i])[0]

        #Fetch data from the GRIB file and assign it to a numpy array for each
        #pressure level.
        u, lats, longs = grb_u.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
        v, lats, longs = grb_v.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
        w, lats, longs = grb_w.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
        
        #Compressing the 2-D numpy arrays into 1-D numpy arrays
        flat_u = u.flatten()
        flat_v = v.flatten()
        flat_w = w.flatten()
        
        #Adjoining pressure level data to the end of the dataframes.
        df_u[str(p_level[i])] = flat_u
        df_v[str(p_level[i])] = flat_v
        df_w[str(p_level[i])] = flat_w
        
    #Generating file names.
    u_out_file = os.path.join(output_dir, 'u.csv')
    v_out_file = os.path.join(output_dir, 'v.csv')
    w_out_file = os.path.join(output_dir, 'w.csv')
    
    #Writing to files.
    df_u.to_csv(u_out_file, index=False)
    df_v.to_csv(v_out_file, index=False)
    df_w.to_csv(w_out_file, index=False)
    
    return "Coordinate transform complete. csv files have been written."
