# -*- coding: utf-8 -*-
"""
This is a data formatting script which extracts long-lat data from ERA5 GRIB1 
and GRIB2 files available in the climate data store and than convert the lat-
long data to a cartesian coordinate system suitable for CARIBOU input.
"""
import pygrib
import numpy as np
import pandas as pd
import os
import barometric_utils as bu

def convert_by_latlong(file, point1, point2, p_level, deltas=[31, 31]):
    """
    This function extracts long-lat data within a bounded region defined by 
    the input parameters. It than converts the long/lat coordinates to an 
    x-y-z cartesian grid where (0,0,0) is located at point1. The resulting 
    coordinate transformed data is output to three csv files formatted to 
    become initial conditions for CARIBOU executions. Latitude must range 
    between [90, -90], where 0 is the equator. Longitude must range 
    between [0, 360], where 0 is the Prime Meridian.
    
    Input parameters:
        file: string of the filename.
        point1: python list where entry 0 is the latitude and entry 1 is the 
        longitude.
        point2: python list where entry 0 is the latitude and entry 1 is the 
        longitude.
        p_level: python list of isobaric pressure levels.
        deltas: python list of coordinate transformation factors to convert 
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
        All output csv files contain the x-y-z coordinates of the data in the 
        first three columns, with the data following in column four.
    """
    
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
            grid_x[i,j] = i*(deltas[0])*(1000)
    
    for i in range(np.shape(lats)[0]):
        for j in range(np.shape(lats)[1]):
            i_ = (np.shape(lats)[0] - i - 1)
            j_ = (np.shape(lats)[1] - j - 1)
            grid_y[i_,j_] = (j_)*(deltas[1])*(1000)
    
    #Flatten both x and y grid components into 1-D arrays
    flat_x = grid_x.flatten()
    flat_y = grid_y.flatten()
    
    #Building the first z component array.
    flat_z_1 = np.full_like(flat_x, (bu.barometric_height(p_level[0]))*1000)
    
    #Building initial dictionaries for the dataframes.
    coords = {'x': flat_x, 'y': flat_y, 'z': flat_z_1}
    
    #Building 3 pandas dataframes to store the data.
    df_u = pd.DataFrame(data=coords)
    df_v = pd.DataFrame(data=coords)
    df_w = pd.DataFrame(data=coords)
    
    #Building 3 pandas dataframes for temporary data storage.
    t_coords = {'x': flat_x, 'y': flat_y}
    t_df_u = pd.DataFrame(data=t_coords)
    t_df_v = pd.DataFrame(data=t_coords)
    t_df_w = pd.DataFrame(data=t_coords)
    
    #Designating a GRIB message for each velocity component at each pressure 
    #level.
    grb_u = grbs.select(name='U component of wind',
                            typeOfLevel='isobaricInhPa',
                            level=p_level[0])[0]
    grb_v = grbs.select(name='V component of wind',
                            typeOfLevel='isobaricInhPa',
                            level=p_level[0])[0]
    grb_w = grbs.select(name='Vertical velocity', 
                            typeOfLevel='isobaricInhPa',
                            level=p_level[0])[0]
    
    #Fetch data from the GRIB file and assign it to a numpy array for each
    #pressure level.
    u, lats, longs = grb_u.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
    v, lats, longs = grb_v.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
    w, lats, longs = grb_w.data(lat1=point1[0], lat2=point2[0], 
                                    lon1=point1[1], lon2=point2[1])
    
    #Adjoining data to the end of the dataframes.
    df_u["data"] = u.flatten()
    df_v["data"] = v.flatten()
    df_w["data"] = w.flatten()
    
    #Loop through each pressure level other than the first.
    for i in range(len(p_level) - 1):
        #Designate GRIB messages for each velocity component at each pressure 
        #level.
        grb_u = grbs.select(name='U component of wind',
                            typeOfLevel='isobaricInhPa',
                            level=p_level[i + 1])[0]
        grb_v = grbs.select(name='V component of wind',
                            typeOfLevel='isobaricInhPa',
                            level=p_level[i + 1])[0]
        grb_w = grbs.select(name='Vertical velocity', 
                            typeOfLevel='isobaricInhPa',
                            level=p_level[i + 1])[0]

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
        
        #Generating a flattened z coordinate.
        flat_z = np.full_like(flat_x, bu.barometric_height(p_level[i + 1]))
        
        #Storing flattened data to the temporary dataframes.
        t_df_u["z"] = flat_z
        t_df_v["z"] = flat_z
        t_df_w["z"] = flat_z
        
        t_df_u["data"] = flat_u
        t_df_v["data"] = flat_v
        t_df_w["data"] = flat_w
        
        #Adjoining temporary dataframes to the output dataframes.
        df_u = df_u.append(t_df_u, ignore_index=True)
        df_v = df_v.append(t_df_v, ignore_index=True)
        df_w = df_w.append(t_df_w, ignore_index=True)
        
    #Generating file names.
    u_out_file = os.path.join(output_dir, 'u.csv')
    v_out_file = os.path.join(output_dir, 'v.csv')
    w_out_file = os.path.join(output_dir, 'w.csv')
    
    #Writing to files.
    df_u.to_csv(u_out_file, index=False)
    df_v.to_csv(v_out_file, index=False)
    df_w.to_csv(w_out_file, index=False)
    
    #Close GRIB file
    grbs.close()
    
    return 'Coordinate transform complete. csv files have been written.'
