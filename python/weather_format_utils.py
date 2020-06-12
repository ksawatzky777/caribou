#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This is a script containing various mathematical functions and utilities 
related to converting lat/long coordinates to a cartesian grid centered on an 
initial point. These functions assume that the earth is a perfect sphere, and 
that the Law of Haversines can be applied.
"""
import math

def hav(theta):
    """
    Function defining the haversine of an angle theta, where theta is in 
    degrees.
    """
    result = (math.sin((theta)*(math.pi)/360))**2
    return result

def calc_great_circle(point1, point2, r_earth): 
    """
    Function computing the great circle distance between two points using the 
    Law of Haversines. point1 is a python list containing the latitude 
    (position 0) and longitude (position 1), point2 is a python list 
    containing the latitude (position 0) and longitude (position 1) and 
    r_earth is a float containing the radius of the earth to be used for the 
    computation.
    """
    d = 2*r_earth*(math.asin(math.sqrt(hav(point2[0] - point1[0]) + 
                                            (math.cos(point1[0]))*
                                            (math.cos(point2[0]))*
                                            (hav(point2[0] - point1[0])))))
    return d

def calc_cart_deltas(point1, point2, r_earth, n_grid_lat, n_grid_long):
    """
    Calculates the distance based resolution of geographical data. 
    point1 is a python list containing the latitude (position 0) and longitude
    (position 1), point2 is a python list containing the latitude (position 0)
    and longitude (position 1), r_earth is a float containing the radius of 
    the earth to be used for the computation, n_grid_lat is an integer 
    representing the number of datapoints along the latitude axis and 
    n_grid_long is an integer representing the the number of datapoints along
    the longitudinal axis. Returns a python list where position 0 is the 
    delta in x and position 1 is the delta in y.
    """
    #Calculating the great circle distances.
    great_circle_y = calc_great_circle([point1[0], 0], [point2[0], 0], r_earth)
    great_circle_x = calc_great_circle([0, point1[1]], [0, point2[1]], r_earth)
    
    #Calculating the grid sizes from the great circle.
    delta_x = (great_circle_x)/(n_grid_long)
    delta_y = (great_circle_y)/(n_grid_lat)
    
    return [delta_x, delta_y]