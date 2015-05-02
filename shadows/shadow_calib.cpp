/**
 *  shadow_calib.cpp
 *
 *  make use of camera calibration data to more precisely estimate
 *   shadow matcher arguments
 */

#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

void ground_plane(float & x, float & y, float u, float v,
                     float tt, float f) {
   float tu = atan2(u, f);
   x = tan(tt + tu);
   y = (v*cos(tu))/(f*cos(tt + tu));
   return;
}

void rc(float r, float u, float v, vector<float> & points) {
   float tu, tv;
   float s, c;
   unsigned index;
   s = sin(r);
   c = cos(r);
   for(index = 0; index < 6; index +=2) {
      tu = u-points[index];
      tv = v-points[index+1];
      points[index] = c*tu - s*tv;
      points[index+1] = c*tv + s*tu;
   }
   return;
}

void vertical_caster(float & z, float ub, float ut,
                        float tt, float f) {
   float tub, tut;
   tub = atan2(ub, f);  tut = atan2(ut, f);
   z = 1.0f - tan(tt + tub)/tan(tt + tut);
   return;
}

int main(int argc, char* argv[]) {
   float roll, tilt, f, u0, v0; // camera calibration information

   vector<float> p(6);
   /* shadow tip in ground plane, caster base in ground plane, caster
      top above ground plane, two coordinates each */

   char c;

   if(argc == 1) {
//      cout << "roll, tilt (degrees); principle point row, principle" <<
//         " point column, focal length" << endl;
      cin >> roll >> tilt >> u0 >> v0 >> f;
//      cout << "shadow tip row, column; caster base row, column; " <<
//         " caster tip row, column" << endl;
      cin >> p[0] >> p[1] >> p[2] >> p[3] >> p[4] >> p[5];
   } else if(argc != 3) {
      cerr << "unexpected argument count; expected\n" <<
         "<invocation> <camera calib. filepath> <points filepath>" <<
         endl;
      return 1;
   } else {
      ifstream fin(argv[1]);
      if(!(fin >> roll >> tilt >> u0 >> v0 >> f) || (fin >> c)) {
         cerr << "malformed camera calibration file; expected\n" <<
            "roll(float, degrees) tilt(float, degrees)\n" <<
            "principle point row(unsigned, pixels) principle point " <<
            "column(unsigned, pixels)\n" <<
            "focal length(float, pixels)" << endl;
         fin.close();
         return 1;
      }
      fin.close();
      fin.open(argv[2]);
      if(!(fin >> p[0] >> p[1] >> p[2] >> p[3] >> p[4] >> p[5]) ||
            (fin >> c)) {
         cerr << "malformed points file; expected\n" <<
            "shadow tip row(unsigned, pixels)\t" <<
            "shadow tip column(unsigned, pixels)\n" <<
            "caster base row(unsigned, pixels)\t" <<
            "caster base column(unsigned, pixels)\n" <<
            "caster tip row(unsigned, pixels)\t" <<
            "caster tip column(unsigned, pixels)" << endl;
         fin.close();
         return 1;
      }
      fin.close();
   }
   /* assertion: roll, tilt, principle point, focal length, and three
      points of interest loaded into respective variables */
   roll *= 3.14159265f/180.0f;
   tilt *= 3.14159265f/180.0f;
   rc(roll, u0, v0, p);

/*   unsigned i;
   for(i = 0; i < p.size(); i += 2) {
      cerr << "(" << p[i] << "," << p[i+1] << ")" << endl;
   }*/

   float xs, ys, xc, yc, zc;
   ground_plane(xs, ys, p[0], p[1], tilt, f);
   ground_plane(xc, yc, p[2], p[3], tilt, f);
   vertical_caster(zc, p[2], p[4], tilt, f);

// if(zc < 0) {
//    xs *= -1;   ys *= -1;
//    xc *= -1;   yc *= -1;   zc *= -1;
// }

//   cerr << "(" << xs << "," << ys << ",0)\n(" << xc << "," << yc << "," << zc << ")" << endl;

   cout << (sqrt(pow((ys-yc),2)+pow((xs-xc),2))/zc) << ' ' <<
      (atan2((ys-yc), (xs-xc))*180.0f/3.14159265f) << endl;
   return 0;
}
