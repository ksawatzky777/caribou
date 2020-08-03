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
import csv

def convert_by_latlong(file, point1, point2, p_level,
                       times, deltas=[31000.0, 31000.0]):
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
        times: python list of times the data is to be extracted from.
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
        coords.csv (csv file containing the coordinate data. X is column 1, y is
        column 2, z is column 3 and t is column 4).

        All output csv files containing weather data use a single column
        format which is accepted by the MOOSE TrilinearInterpolation object
        (required for CARIBOU's scalar transport kernels). If more than 1
        timestep of the velocity vector is required, the data for the additional
        timesteps are adjoined as columns with a header containing the name of
        the data and its timestep (Ex. w3: where w is the data type and 3 is the
        timestep).

        coords.csv is formatted such that each column has the same number of
        rows (due to the limitation of the csv file format). The largest
        dimension is the only dimension filled completely with real coordinate
        values, the remainder are filled with zeros and actual coordinate
        values. CARIBOU's file reading scheme can decode the data and remove
        these superfluous zeroes (using the GenericCaribousMaterial or the
        STMaterial materials).
    """

    #Declaring output directory
    output_dir = './'

    print ('Opening file.')
    #Open the GRIB file.
    grbs = pygrib.open(file)

    #Fetch initial latitudes and longitudes for looping purposes.
    grb_u = grbs.select(name='U component of wind',
                        typeOfLevel='isobaricInhPa',
                        level=p_level[0])[0]

    temp_u, lats, longs = grb_u.data(lat1=point1[0], lat2=point2[0],
                                     lon1=point1[1], lon2=point2[1])

    print ('Building coordinate system.')
    #Building cartesian coordinate system from the latitudes, longitudes and
    #pressure levels.
    grid_x = np.zeros(np.shape(longs)[0])
    grid_y = np.zeros(np.shape(lats)[1])
    grid_z = np.zeros(len(p_level))
    grid_t = np.zeros(len(times))

    for x in range(np.shape(longs)[0]):
        grid_x[x] = x*deltas[0]

    for y in range(np.shape(lats)[1]):
        grid_y[y] = y*deltas[1]

    for z in range(len(p_level)):
        grid_z[z] = bu.barometric_height(p_level[z])*1000

    for t in range(len(times)):
        grid_t[t] = times[t]

    #Filling the 3 smallest dimensions with zeroes to match the largest.
    largest = 0
    if np.shape(grid_x)[0] > largest:
        largest = np.shape(grid_x)[0]

    if np.shape(grid_y)[0] > largest:
        largest = np.shape(grid_y)[0]

    if np.shape(grid_z)[0] > largest:
        largest = np.shape(grid_z)[0]

    if np.shape(grid_t)[0] > largest:
        largest = np.shape(grid_t)[0]

    out_x = np.zeros(largest)
    out_y = np.zeros(largest)
    out_z = np.zeros(largest)
    out_t = np.zeros(largest)

    for x in range(np.shape(longs)[0]):
        out_x[x] = grid_x[x]

    for y in range(np.shape(lats)[1]):
        out_y[y] = grid_y[y]

    for z in range(len(p_level)):
        out_z[z] = grid_z[z]

    for t in range(len(times)):
        out_t[t] = grid_t[t]

    #Building a pandas dataframe for the output coordinates.
    coords = {'x': out_x, 'y': out_y, 'z': out_z, 't': out_t}

    df_coords = pd.DataFrame(data=coords)

    print ('Extracting initial data at ' + str(times[0]) + ' (time zero).')
    #Loop over the data at the initial time index.
    #Preallocating for speed.
    data_size = np.shape(grid_x)[0]*np.shape(grid_y)[0]*np.shape(grid_z)[0]
    u = np.zeros(data_size)
    v = np.zeros(data_size)
    w = np.zeros(data_size)

    #Counter for the master array position.
    counter = 0

    #Loop over the data for u.
    for x in range(np.shape(longs)[0]):
        for y in range(np.shape(lats)[1]):
            for z in range(len(p_level)):

                #Fetch grib message for u.
                grb_u = grbs.select(name='U component of wind',
                                    typeOfLevel='isobaricInhPa',
                                    level=p_level[z])[0]
                #Extract the data for u (lats and longs come along for the
                #ride).
                temp_u, lats, longs = grb_u.data(lat1=point1[0],
                                                 lat2=point2[0],
                                                 lon1=point1[1],
                                                 lon2=point2[1])
                #Collapse the data into a 1-D array as per MOOSE
                #TrilinearInterpolation specification.
                u[counter] = temp_u[x,y]
                counter += 1

    print ('U completed at ' + str(times[0]) + ' (time 0).')
    counter = 0

    #Loop over the data for v. Same process as u.
    for x in range(np.shape(longs)[0]):
        for y in range(np.shape(lats)[1]):
            for z in range(len(p_level)):

                grb_v = grbs.select(name='V component of wind',
                                    typeOfLevel='isobaricInhPa',
                                    level=p_level[z])[0]
                temp_v, lats, longs = grb_v.data(lat1=point1[0],
                                                 lat2=point2[0],
                                                 lon1=point1[1],
                                                 lon2=point2[1])
                v[counter] = temp_v[x,y]
                counter += 1

    print ('V completed at ' + str(times[0]) + ' (time 0).')
    counter = 0

    #Loop over the data for w. Same process as u.
    for x in range(np.shape(longs)[0]):
        for y in range(np.shape(lats)[1]):
            for z in range(len(p_level)):

                grb_w = grbs.select(name='Vertical velocity',
                                    typeOfLevel='isobaricInhPa',
                                    level=p_level[z])[0]
                temp_w, lats, longs = grb_w.data(lat1=point1[0],
                                                 lat2=point2[0],
                                                 lon1=point1[1],
                                                 lon2=point2[1])
                w[counter] = bu.vert_to_w(temp_w[x,y])
                counter += 1

    print ('W completed at ' + str(times[0]) + ' (time 0).')

    #Building pandas dataframes for each component of the velocity.
    df_u = pd.DataFrame(u, columns=['u0'])
    df_v = pd.DataFrame(v, columns=['v0'])
    df_w = pd.DataFrame(w, columns=['w0'])

    #Loop over each time index other than the first.
    for t in range(len(times) - 1):
        print ('Extracting data at ' + str(times[t]) +
               ' (time ' + str(t) + ').')

        data_size = np.shape(grid_x)[0]*np.shape(grid_y)[0]*np.shape(grid_z)[0]
        u = np.zeros(data_size)
        v = np.zeros(data_size)
        w = np.zeros(data_size)

        counter = 0

        #Loop over the data for u.
        for x in range(np.shape(longs)[0]):
            for y in range(np.shape(lats)[1]):
                for z in range(len(p_level)):
                    grb_u = grbs.select(name='U component of wind',
                                        typeOfLevel='isobaricInhPa',
                                        level=p_level[z])[t + 1]
                    temp_u, lats, longs = grb_u.data(lat1=point1[0],
                                                     lat2=point2[0],
                                                     lon1=point1[1],
                                                     lon2=point2[1])

                    u[counter] = temp_u[x,y]
                    counter += 1

        print ('U completed at ' + str(times[t]) + ' (time ' + str(t) + ').')
        counter = 0

        #Loop over the data for v.
        for x in range(np.shape(longs)[0]):
            for y in range(np.shape(lats)[1]):
                for z in range(len(p_level)):

                    grb_v = grbs.select(name='V component of wind',
                                        typeOfLevel='isobaricInhPa',
                                        level=p_level[z])[t + 1]
                    temp_v, lats, longs = grb_v.data(lat1=point1[0],
                                                     lat2=point2[0],
                                                     lon1=point1[1],
                                                     lon2=point2[1])
                    v[counter] = temp_v[x,y]
                    counter += 1

        print ('V completed at ' + str(times[t]) + ' (time ' + str(t) + ').')
        counter = 0

        #Loop over the data for w.
        for x in range(np.shape(longs)[0]):
            for y in range(np.shape(lats)[1]):
                for z in range(len(p_level)):

                    grb_w = grbs.select(name='Vertical velocity',
                                        typeOfLevel='isobaricInhPa',
                                        level=p_level[z])[t + 1]
                    temp_w, lats, longs = grb_w.data(lat1=point1[0],
                                                     lat2=point2[0],
                                                     lon1=point1[1],
                                                     lon2=point2[1])
                    w[counter] = bu.vert_to_w(temp_w[x,y])
                    counter += 1

        print ('W completed at ' + str(times[t]) + ' (time ' + str(t) + ').')

        t_df_u = pd.DataFrame(u, columns=['u' + str(t)])
        t_df_v = pd.DataFrame(v, columns=['v' + str(t)])
        t_df_w = pd.DataFrame(w, columns=['w' + str(t)])

        #Adjoining temporary dataframes to the output dataframes.
        df_u = df_u.append(t_df_u, ignore_index=True)
        df_v = df_v.append(t_df_v, ignore_index=True)
        df_w = df_w.append(t_df_w, ignore_index=True)

    #Close GRIB file
    grbs.close()

    #Generating file names.
    u_out_file = os.path.join(output_dir, 'u.csv')
    v_out_file = os.path.join(output_dir, 'v.csv')
    w_out_file = os.path.join(output_dir, 'w.csv')
    coords_out_file = os.path.join(output_dir, 'coords.csv')

    print('Writing to csv files.')

    #Writing to files.
    df_u.to_csv(u_out_file, index=False)
    df_v.to_csv(v_out_file, index=False)
    df_w.to_csv(w_out_file, index=False)
    df_coords.to_csv(coords_out_file, index=False)

    return 'Coordinate transform complete. csv files have been written.'
