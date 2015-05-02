/**
 *  shadow.h
 *
 *  header file for shadow processing for image orientation
 */

#ifndef SHADOW_HEADER
#define SHADOW_HEADER

#include "shadow_util.h"

/* convert May 5 (5, 5) or July 3 (7, 3) to a useful form for t_year */
float date_to_tyear(unsigned char month, unsigned char day);

/* convert t_day to human readable time */
void tday_to_time(float t_day,
                     unsigned char & hour, unsigned char & minute);

/* convert t_year to human readable date */
void tyear_to_date(float t_year,
                     unsigned char & month, unsigned char & day);

/* time of day in hours since noon (morning -> negative values) *
 * time of year in days since September equinox (date_to_tyear) *
 * latitude in DEGREES                                          *
 * returns the angle in DEGREES that shadows make with NORTH;   *
 *  WEST is 90, SOUTH is 180, EAST is -90                       */
float times_and_lat_to_shadow(float t_day, float t_year, float lat);
/* orientation and length are return variables                *
 * orientation is the direction, length is actually the angle *
 *  that the shadow makes with the vertical                   *
 * both are in RADIANS, since this is intended as an internal */
void times_and_lat_to_shadow(float t_day, float t_year, float lat,
                              float & orientation, float & length);

/* t_year is a sequence of times of year (see date_to_tyear); due   *
 *  to certain shadows being impossible at certain times of year,   *
 *  t_year will be modified                                         *
 * latitude is in DEGREES                                           *
 * shadow is the angle between the shadow and vertical in the query *
 *  in RADIANS, because you're getting it from an atan anyway       *
 * t_day is used for output, and will contain the same number of    *
 *  elements as t_year, with corresponding elements identifying a   *
 *  time that could generate a shadow of the given length           *
 *  the initial value of t_day is used to bound both the search and *
 *  the output spaces                                               */
void year_lat_shadow_to_day(range & t_year,
                              float lat, float shadow,
                              range & t_day);
/* orientation is an optional variable for returning  *
 *  the angles (RADIANS) of the corresponding shadows */
void year_lat_shadow_to_day(range & t_year,
                              float lat, float shadow,
                              range & t_day, range & orientation);

/* Why the September equinox?  Because it makes the math easier: in *
 * the SOLAR coordinate system (origin at center of EARTH, SUN on   *
 * y-axis, path of EARTH's orbit on x-axis, SOLAR NORTH on z-axis), *
 * the axial tilt of the EARTH has its greatest component along the *
 * x-axis and none along the y-axis.  Unfortunately, I still need   *
 * to negate the angular_sin value, because it decreases, rather    *
 * increasing, from that point.                                     */
#endif
