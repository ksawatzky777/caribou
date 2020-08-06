# -*- coding: utf-8 -*-
"""
This is adata formatting script which extracts lat-long data from MERRA-2
netCDF files available through NASA's GMAO project and than converts the lat-
long data to a cartesian coordinate system suitable for CARIBOU input. This
script is defunct as the data formatting for CARIBOU has changed.
"""

import netCDF4 as ncd
import numpy as np
import pandas as pd
import os
import barometric_utils as bu

def convert_by_latlong(file, point1, point2, deltas=[70, 50]):
    """
    This function extracts long-lat data within a bounded region defined by
    the input parameters. It than converts the long/lat coordinates to an
    x-y-z cartesian grid where (0,0,0) is located at point1. The resulting
    coordinate transformed data is output to three csv files formatted to
    become initial conditions for CARIBOU executions. Latitude must range
    between [90, -90], where 0 is the equator. Longitude must range
    between [-180, 180], where 0 is the Prime Meridian.

    Input parameters:
        file: string of the filename.
        point1: python list where entry 0 is the latitude and entry 1 is the
        longitude.
        point2: python list where entry 0 is the latitude and entry 1 is the
        longitude.
        deltas: python list of coordinate transformation factors to convert
        lat/long values to x (entry 0) and y (entry 1) values. By default,
        this is 70 km between lat datapoints and 50 km between long datapoints
        (default for MERRA-2 reanalysis)
    Outputs:
        u.csv (csv file containing the magnitude of the east-facing wind
        component, m/s).
        v.csv (csv file containing the magnitude of the north-facing wind
        component), m/s.
        w.csv (csv file containing the magnitude of the updraft velocity,
        Pa/s).
        All output csv files also contain the x-y coordinates of each
        datapoint in columns 1 and 2 (km). The remaining data is contained in
        columns 1 -> n, where n is the number of pressure levels. The header
        for each column is the z coordinate of the pressure level (km).
    """

    print('Converting data, this will take some time')

    #Declaring the output directory.
    output_dir = './'

    #Compute data indicies.
    j_min = int((point1[0] + 90)/(deltas[0]) + 1)
    j_max = int((point2[0] + 90)/(deltas[0]) + 1)
    n_j = j_max - j_min

    i_min = int((point1[1] + 180)/(deltas[1]) + 1)
    i_max = int((point2[1] + 180)/(deltas[1]) + 1)
    n_i = i_max - i_min

    #Opening the netCDF file.
    data_f = ncd.Dataset(file, mode='r')

    #Extract altitudes.
    p_level = data_f.variables['lev'][:]

    #Declaring numpy arrays for the coordinate system.
    grid_x = np.ones(n_j, n_i)
    grid_y = np.ones(n_j, n_i)

    #Generating the 2-D cartesian grid from lats and longs.
    for i in range(np.shape(grid_x)[0]):
        for j in range(np.shape(grid_x)[1]):
            grid_x[i,j] = i*(deltas[0])

    for i in range(np.shape(grid_y)[0]):
        for j in range(np.shape(grid_y)[1]):
            i_ = (np.shape(grid_y)[0] - i - 1)
            j_ = (np.shape(grid_y)[1] - j - 1)
            grid_y[i_,j_] = (j_)*(deltas[1])

    #Flatten both x and y grid components into 1-D arrays.
    flat_x = grid_x.flatten()
    flat_y = grid_y.flatten()

    #Building initial dictionaries for the dataframes.
    coords = {'x': flat_x, 'y': flat_y}

    #Building 3 pandas dataframes to store the data.
    df_u = pd.DataFrame(data=coords)
    df_v = pd.DataFrame(data=coords)
    df_w = pd.DataFrame(data=coords)

    #Loop through all the pressure levels.
    for i in range(np.shape(p_level)[1]):
        #Extract variables
        u = data_f.variables['U'][0,i,j_min:j_max,i_min:i_max]
        v = data_f.variables['V'][0,i,j_min:j_max,i_min:i_max]
        w = data_f.variables['OMEGA'][0,i,j_min:j_max,i_min:i_max]

        #Compressing the 2-D numpy arrays into 1-D numpy arrays
        flat_u = u.flatten()
        flat_v = v.flatten()
        flat_w = w.flatten()

        #Adjoining pressure level data to the end of the dataframes.
        df_u["z=" + str(bu.barometric_height(p_level[i]))] = flat_u
        df_v["z=" + str(bu.barometric_height(p_level[i]))] = flat_v
        df_w["z=" + str(bu.barometric_height(p_level[i]))] = flat_w

    #Generating file names.
    u_out_file = os.path.join(output_dir, 'u.csv')
    v_out_file = os.path.join(output_dir, 'v.csv')
    w_out_file = os.path.join(output_dir, 'w.csv')

    #Writing to files.
    df_u.to_csv(u_out_file, index=False)
    df_v.to_csv(v_out_file, index=False)
    df_w.to_csv(w_out_file, index=False)

    #Close netCDF file.
    data_f.close()

    return 'Coordinate transform complete. csv files have been written.'
