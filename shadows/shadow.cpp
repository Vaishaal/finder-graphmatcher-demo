/**
 *  shadow.cpp
 *
 *  definitions for shadow analysis for image orientation
 */

#include <cmath>

#include "shadow.h"

using namespace std;
const float pi = 3.14159265f;
const float tilt = 23.4*pi/180;
 
float date_to_tyear(unsigned char month, unsigned char day) {
   const int sept20 = 290;
   int result = (int)(month*30 + day) - sept20;
   return (float)((result >= 0)?(result):(result+360));
}

void tday_to_time(float t_day,
                     unsigned char & hour, unsigned char & minute) {
   hour = (unsigned char)(t_day + 12);
   while(t_day >= 1.0f) { t_day -= 1.0f; }
   while(t_day < 0.0f) { t_day += 1.0f; }
   minute = (unsigned char)(t_day*60);
   return;
}

void tyear_to_date(float t_year,
                     unsigned char & month, unsigned char & day) {
   int temp = ((int)(t_year)) + 290;
   /* could be smarter here, but don't expect multiple iterations */
   while(temp <= 0) { temp += 360; }
   while(temp > 360) { temp -= 360; }
   month = temp/30;
   day = temp%30;
   if(day == 0) {
      day = 30;
      month--;
   }
   if(month == 0) { month = 12; }
   return;
}

float times_and_lat_to_shadow(float t_day, float t_year, float lat) {
   float orientation, length;
   times_and_lat_to_shadow(t_day, t_year, lat, orientation, length);
   return orientation*180/pi;
}

void times_and_lat_to_shadow(float t_day, float t_year, float lat,
                              float & orientation, float & length) {
   /* a couple indices */
   unsigned i, j;

   /* conversion to radians */
   t_day *= 15*pi/180;
   t_year *= pi/180;
   lat *= pi/180;

   /* some coordinate-conversion stuff */
   float ps = angular_cos(tilt, t_year); /* phi_s */
   /* float pe = -angular_sin(tilt, t_year); */ /* phi_e */
   float pe = atan2(-sin(tilt)*sin(t_year),
                     sqrt(1 - pow(sin(tilt)*sin(t_year), 2)));
   float qs1; /* query point in solar coordinates, coordinate[1] */

   std::vector<float> e2s(9), s2e(9); /* EARTH to SOLAR, SOLAR to EARTH */
   e2s[0] = cos(ps); e2s[1] = sin(ps)*sin(pe);  e2s[2] = -sin(ps)*cos(pe);
   e2s[3] = 0;       e2s[4] = cos(pe);          e2s[5] = sin(pe);
   e2s[6] = sin(pe); e2s[7] = -cos(ps)*sin(pe); e2s[8] = cos(ps)*cos(pe);
   for(i = 0; i < 3; i++) {
      for(j = 0; j < 3; j++) {
         s2e[i+j*3] = e2s[j+i*3];
      }
   }
   std::vector<float> e2g(9); /* EARTH to GROUND */
   e2g[0] = cos(t_day);           e2g[1] = sin(t_day);           e2g[2] = 0;
   e2g[3] = -cos(lat)*sin(t_day); e2g[4] = cos(lat)*cos(t_day);  e2g[5] = sin(lat);
   e2g[6] = sin(lat)*sin(t_day);  e2g[7] = -sin(lat)*cos(t_day); e2g[8] = cos(lat);

   /* one last bit of setup: the query point in the EARTH coordinate frame */
   std::vector<float> qe(3), qss, se, sg;
   qe[0] = -cos(lat)*sin(t_day); qe[1] = cos(lat)*cos(t_day); qe[2] = sin(lat);

   /* THROW MATH AT IT! */
   rotate(e2s, qe, qss);
   qs1 = qss[1];
   qss[1] = -(qss[0]*qss[0] + qss[2]*qss[2])/qss[1]; /* y' = (x^2 + z^2)/(-y) */
   rotate(s2e, qss, se);
   rotate(e2g, se, sg);

   /* WEST: sg[0], NORTH: sg[2] */
   if(qss[1] < qs1) {
      orientation = atan2(sg[0], sg[2]);
      length = atan(sqrt(sg[0]*sg[0] + sg[2]*sg[2]));
   } else {
      orientation = (atan2(sg[0], sg[2]) > 0)?
                     (atan2(sg[0], sg[2]) - pi):
                     (atan2(sg[0], sg[2]) + pi);
      length = pi/2;
   }
   return;
}

void year_lat_shadow_to_day(range & t_year,
                              float lat, float shadow,
                              range & t_day) {
   range orientation;
   year_lat_shadow_to_day(t_year, lat, shadow, t_day, orientation);
   return;
}

void year_lat_shadow_to_day(range & t_year,
                              float lat, float shadow,
                              range & t_day, range & orientation) {
   const float eps = 0.001f;
   range year_times(t_year), day_times(t_day);
   t_year = t_day = orientation = range();

   float year, day, len, orient;
   year_times.init();
   while(year_times.has_next()) {
      year = year_times.next();
      day_times.init();
      while(day_times.has_next()) {
         day = day_times.next();
         times_and_lat_to_shadow(year, day, lat, orient, len);
         len -= shadow;
         len *= len;
         if(len < eps) {
            t_year.add(year); t_day.add(day);   orientation.add(orient);
         }
      }
   }
   return;
}

/* // COMMENTED OUT because it doesn't work:
   //  can't handle times of year that can't generate shadows of the
   //  given length, can't handle multiple times during the same day
   //  where the shadow is the same length, and risked oscillation
void year_lat_shadow_to_day(range & t_year,
                              float lat, float shadow,
                              range & t_day) {
   // execution controls
   const unsigned iter_cap = 100;
   const unsigned osc_cap = 5;
   const float eps = 0.01;

   // some constants
   float sl, cl;
   sl = sin(lat*pi/180); cl = cos(lat*pi/180);

   // precondition enforcement
   t_day = range();

   // pseudo-constants (constant for a single t_year)
   float y, ps, pt, ss, cs, st, ct;

   // iteration variables
   float day, d1, d2, p, sp, cp, sd, cd;
   unsigned i, osc;

   // for each time of year
   t_year.init();
   while(t_year.has_next()) {
      y = t_year.next();
      ps = angular_cos(tilt, y*pi/180);
      pt = -angular_sin(tilt, y*pi/180);
      ss = sin(ps);  cs = cos(ps);  st = sin(pt);  ct = cos(pt);

      p = angular_pythagorus(shadow, asin(cs*ct*sl - cs*st*cl));
      sp = sin(p);   cp = cos(p);
      d1 = day = asin( (ct*cl*sp + st*sl*sp - ss*st*cl*cp + ss*ct*sl*cp)
                        / (cs*cl*cp) );
      // do fixed-point iteration
      for(i = 1; i < iter_cap && d1 > eps; i++) {
         d2 = d1;
         sd = sin(day); cd = cos(day);
         p = angular_pythagorus(shadow, asin(ss*cl*sd + cs*ct*sl - cs*st*cl*cd));
         sp = sin(p);   cp = cos(p);
         d1 = day;
         day = asin( (ct*cl*cd*sp + st*sl*sp - ss*st*cl*cd*cp + ss*ct*sl*cp)
                     / (cs*cl*cp) );
         d1 = day - d1;
         if(d1*d2 < 0 && d1*d1 > d2*d2) {
            osc++;
            cout << "shadow.cpp::year_lat_shadow_to_day():\n\t";
            cout << "oscillation detected (" << osc << ")" << endl;
            if(osc > osc_cap) {
               cerr << "shadow.cpp::year_lat_shadow_to_day():\n\t";
               cerr << "ABORTED: oscillation persisted for arguments:\n\t";
               cerr << "latitude:\t" << lat << "\tt_year:\t" << y;
               cerr << "\tshadow:\t" << shadow*180/pi << endl;
               day = 0;
               break;
            }
         } else {
            osc = 0;
         }
      }

      // store in t_day
      t_day.add(day*180*15/pi);
   }
}
*/
