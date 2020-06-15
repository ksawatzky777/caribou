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
    Function defining the haversine of an angle theta.
    
    Input parameters:
        theta: Angle between two points, in degrees.
    Returns:
        Haversine of theta.
    """
    return (math.sin((theta)*(math.pi)/360))**2

def calc_great_circle(point1, point2, r_earth): 
    """
    Function computing the great circle distance between two points using the 
    Law of Haversines. 
    
    Input Parameters:
        point1: python list containing the latitude (position 0) and longitude
        (position 1).
        point2: python list containing the latitude (position 0) and longitude 
        (position 1).
        r_earth: float containing the radius of the earth to be used for the 
        computation (any unit of distance is acceptable).
    Returns:
        The great circle distance between point1 and point2, with units 
        equivalent to the units of r_earth.
    """
    return 2*r_earth*(math.asin(math.sqrt(hav(point2[0] - point1[0]) + 
                                            (math.cos(point1[0]))*
                                            (math.cos(point2[0]))*
                                            (hav(point2[0] - point1[0])))))

def calc_cart_deltas(point1, point2, r_earth, n_grid_lat, n_grid_long):
    """
    Calculates the distance-based resolution of geographical data.
    
    Input Parameters:
        point1: python list containing the latitude (position 0) and longitude
        (position 1)
        point2: python list containing the latitude (position 0) and longitude 
        (position 1)
        r_earth: float containing the radius of the earth to be used for the 
        computation (any unit of distance is acceptable). 
        n_grid_lat: integer representing the number of datapoints along the 
        latitude axis.
        n_grid_long: integer representing the the number of datapoints along
        the longitudinal axis. 
    Returns: 
        python list where position 0 is the delta in x and position 1 is the 
        delta in y.
    """
    #Calculating the great circle distances.
    great_circle_y = calc_great_circle([point1[0], 0], [point2[0], 0], r_earth)
    great_circle_x = calc_great_circle([0, point1[1]], [0, point2[1]], r_earth)
    
    #Calculating the grid sizes from the great circle.
    delta_x = (great_circle_x)/(n_grid_long)
    delta_y = (great_circle_y)/(n_grid_lat)
    
    return [delta_x, delta_y]