#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
This is a utility script which converts isobaric pressure levels utilized by 
ERA5 and MERRA-2 for non-surface computations into the respective altitudes 
relative to the surface of the earth.
"""

import math

def barometric_height(pressure):
    """
    This function computes the height of a column of air above the earths 
    surface using the barometric formula.
    
    Input parameters:
        pressure: Pressure to be converted to a height (in hPa).
    Returns:
        Height of the air column, in km.
    """
    #Define level dependant constants.
    p_b = [1013.25, 226.321, 54.7489, 8.6802, 1.1091, 0.6694, 0.0396]
    h_b = [11000, 20000, 32000, 47000, 51000, 71000]
    t_b = [288.15, 216.65, 216.15, 228.65, 270.65, 270.65, 214.65]
    l_b = [-0.0065, 0.0, 0.001, 0.0028, 0.0, -0.0028, -0.002]
    
    #Define constants.
    g = 9.8067
    gas_const = 8.3145
    molar_mass = 0.0290
    
    #Determine if a given pressure is directly equivalent to a definition 
    #pressure.
    if pressure in p_b:
        #If the pressure is a definition pressure, returns the related 
        #definition height.
        return h_b[p_b.index(pressure)]/1000
    
    #Determine which section of the piecewise-exponential function is relevant
    #for the given pressure.
    if pressure < p_b[0] and pressure > p_b[1]:
        b = 0
    elif pressure < p_b[1] and pressure > p_b[2]:
        b = 1
    elif pressure < p_b[2] and pressure > p_b[3]:
        b = 2
    elif pressure < p_b[3] and pressure > p_b[4]:
        b = 3
    elif pressure < p_b[4] and pressure > p_b[5]:
        b = 4
    elif pressure < p_b[5] and pressure > p_b[6]:
        b = 5
    elif pressure < p_b[6]:
        b = 6
    else:
        raise ValueError('The barometric formula is not valid for this case')
    
    if l_b[b] != 0.0:
        #Compute the height of the column of air for a non-zero temperature 
        #lapse rate, returning it in km.
        exponent = (gas_const*l_b[b])/(g*molar_mass)
        denom = (pressure/p_b[b])**(exponent)
    
        return (h_b[b] + ((t_b[b]/denom) - t_b[b])/(l_b[b]))/1000
    else:
        #Compute the height of the column of air for a temperature lapse rate
        #which is equal to zero.
        log = math.log(pressure/(p_b[b]))
        denom = (gas_const*(t_b[b]))/((-g)*molar_mass)
        
        return (log*denom + h_b[b])/1000
