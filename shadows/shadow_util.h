/**
 *  shadow_util.h
 *
 *  Utility functionality for shadow analysis: range objects and
 *   matehmatical heavy lifting
 */

#ifndef SHADOW_UTIL_HEADER
#define SHADOW_UTIL_HEADER

#include <vector>

class range {
public:
/* constructors, destructor, and copy-equals */
   range();
   range(float lo, float hi, unsigned count);
   range(const std::vector<float> & v);
   range(const range & other);
   ~range();
   range operator=(const range & other);

/* accessors; assumes that range is not null, outputs garbage if null */
   float high() const;
   float low() const;
   /* cardinality, or number of elements */
   float card() const;

/* iteration accessors */
   void init();
   bool has_next();
   float next();

/* predicates */
   /* is the range empty, containing no points? */
   bool empty() const;
   /* does it contain only a single point? */
   bool singular() const;
   /* does it contain a specific point? */
   bool contains(float f) const;

/* mutators */
   void add(float f);
   void sort();

/* interactions with other range objects; in both cases, *
 *  other may be modified, but this will not be          */
   void intersect(range & other);
   void unite(std::vector<range*> & others);
private:
   float my_low;
   float my_high;
   std::vector<float>* values;
   unsigned index;

   void sort(unsigned start, unsigned end);
};

/* not quite a member function, but still range related */
void sort_range_vector(std::vector<range*> & to_sort);

/* compute the angular hypotenuse: the angle between the non-adjacent *
 *  legs of adjacent angles in perpendicular planes                   */ 
float angular_hypotenuse(float a, float b);

/* essentially the inverse of the above; compute the missing leg *
 *  angle; infeasible/nonexistent results have value -1          */
float angular_pythagorus(float c, float a);

/* compute the observed angle between two vectors when the *
 *  actual angle is c and the angle between the plane      *
 *  containing them and the image plane is a               */
float angular_cos(float c, float a);

/* compute the angle perpendicular to observed angle mentioned above */
float angular_sin(float c, float a);

/* apply a rotation matrix */
void rotate(const std::vector<float> & rotation,
               const std::vector<float> & point,
               std::vector<float> & rotated_point);

#endif
