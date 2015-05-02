/**
 *  shadow_main.cpp
 *
 *  analyst interface for shadow analysis
 */

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "shadow.h"

using namespace std;

const float pi = 3.14159265f;
const float y_res = 0.5f;
const float d_res = 1.0f/30.0f;
const float l_res = 1.0f*pi/180.0f;
const float o_res = 5.0f*pi/180.0f;

/* handle wraparound effects */
float get(vector<float> & v, unsigned u) {
   return (u >= v.size())?(get(v, u - v.size()) + 2*pi):(v[u]);
}

/* on-line insertion sort */
void insert(vector<float> & v, float f) {
   unsigned u = v.size();
   v.push_back(f);
   for( ; u > 0 && v[u] < v[u-1]; u--) {
      f = v[u];   v[u] = v[u-1]; v[u-1] = f;
   }
   return;
}

/* get to range [0, 2*pi) */
float mod_2pi(float f) {
   while(f < 0) { f += 2*pi; }
   while(f >= 2*pi) { f -= 2*pi; }
   return f;
}

int main(int argc, char* argv[]) {
/*   if(argc < 4 || argc > 5) {
      cout << "<invocation> <latitude> <shadow_length/caster_height> <offset> [\"summer\"|\"winter\"]" << endl;
      return 0;
   }*/

   float lat, len, off;
   if(argc < 4 || argc > 5) {
      cout << "[(0.0,360.0)]" << endl;
      return 0;
   } else {
      lat = atof(argv[1]);
      len = atan(atof(argv[2]));
      off = pi*atof(argv[3])/180.0f;
   }
   vector<float> t_year, t_day, matches;
   //vector<unsigned> counts(((unsigned)(2.0f*pi/o_res)), 0);
   float f, o, l, v, mx, mn;
   unsigned i, j, c1, c2;
   if(argc == 4 || ( (argv[4][0] == 'w') ^ (lat < 0) )) {
      for(f = 45.0f; f < 225.0f; f += y_res) {
         t_year.push_back(f);
      }
   }
   if(argc == 4 || ( (argv[4][0] == 's') ^ (lat < 0) )) {
      for(f = 225.0f; f < 405.0f; f += y_res) {
         t_year.push_back(f);
      }
   }
   for(f = -5.0f; f <= 5.0f; f += d_res) {
      t_day.push_back(f);
   }

   for(i = 0; i < t_year.size(); i++) {
      for(j = 0; j < t_day.size(); j++) {
         times_and_lat_to_shadow(t_day[j], t_year[i], lat, o, l);
         if(abs(len - l) <= l_res) {
            insert(matches, mod_2pi(off-o));
         }
      }
   }

//   cout << "arguments:\t" << argv[1] << ", " << argv[2] << ", " << argv[3];
   if(argc > 4) {
      cout << ", " << argv[4];
   }
//   cout << "\n" << matches.size() << " matching lengths considered." << endl;
   /* start analysis at a gap */
   for(i = 1;
         i < matches.size() && (matches[i]-matches[i-1]) < o_res;
         i++);
   if(i == matches.size() && (matches[0] + 2*pi - matches[i-1]) < o_res) {
      /* handle a full wraparound, doesn't do mean or variance */
      cout << "[(0.0,360.0)]" << endl;
   } else {
      /* i is at the ccw edge of a cluster */
      c1 = 0;
      cout << '[';
      while (c1 < matches.size()) {
         for(f = mn = get(matches, i), v = pow(get(matches, i), 2), c2 = 1, i++;
               (get(matches, i) - get(matches, i-1)) < o_res;
               f += get(matches, i), v += pow(get(matches, i), 2), c2++, i++);
         mx = get(matches, i-1);
         c1 += c2;
         cout << '(' << (mod_2pi(mn)*180.0f/pi) << ',' << (mod_2pi(mx)*180.0f/pi) << "),";
/*         cout << (c2*100.0f/matches.size()) << "\% of matches;\n\t";
         cout << "mean:   \t" << (mod_2pi(f/c2)*180.0f/pi) << "\n\tstd. dev:\t";
         cout << (sqrt((v/c2) - pow((f/c2), 2))*180.0f/pi) << endl;*/
      }
      cout << "\b]" << endl;
   }

/*   for(i = 0; i < matches.size(); i++) {
      counts[(unsigned)((matches[i]+pi)/o_res)]++;
   }
   for(i = 0; i < counts.size(); i++) {
      if(counts[i] > 0) {
         cout << "\t[" << (off+180.0f-(i*o_res*180.0f/pi)) << ",";
         cout << (off+180.0f-((i+1)*o_res*180.0f/pi)) << "]:\t";
         cout << ((float)counts[i])/((float)matches.size()) << endl;
      }
   }*/

   return 0;
}
