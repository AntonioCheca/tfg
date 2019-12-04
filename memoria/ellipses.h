#ifndef ELLIPSESH
#define ELLIPSESH

#include <cmath>
#include "hittable.h"
#include "onb.h"
#include "pdf.h"
#include "random.h"

/** Clase elipse, subclase de hittable, que representa el modelo de una elipse con generacion de puntos en funcion del area.
  */
class ellipse: public hittable  {
    public:
        ellipse() {}
        /// El constructor. Necesita un centro, los dos ejes y el material.
        ellipse(vec3 cen, vec3 ax1, vec3 ax2, material *m) : center(cen), axis1(ax1), axis2(ax2), mat_ptr(m)  {calcPerp();
          x0 = center.x()-axis1.x();
          x1 = center.x()+axis1.x();
          z0 = center.z()-axis2.z();
          z1 = center.z()+axis2.z();
          k = center.y();
        };
        /// Funcion que calcula si un rayo r interseca con la elipse en [tmin, tmax], y guarda los datos de interseccion en el hit_record rec.
        virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
        /// Calcula la caja que engloba a la elipse. Como es dos dimensional, la caja anade "altura" en el eje en el que la elipse es plana.
        virtual bool bounding_box(float t0, float t1, aabb& box) const;
        /// Genera la funcion de densidad del punto elegido con random. Es en funcion del area asi que depende de la distancia del punto.
        virtual float  pdf_value(const vec3& o, const vec3& v) const;
        /// Genera un vector desde el punto o hacia un punto escogido de forma uniforme en la elipse.
        virtual vec3 random(const vec3& o);
        /// Calcula el normal a la elipse y lo guarda en perp.
        void calcPerp(){
          perp = cross(axis1, axis2);
          perp.make_unit_vector();
        }
        /// Centro de la elipse
        vec3 center;
        /// Ejes de la elipse
        vec3 axis1, axis2;
        /// Vector perpendicular a la elipse.
        vec3 perp;
        /// Los extremos de la caja que engloba a la elipse.
        float x0, x1, z0, z1, k;
        /// Material de la elipse, todo hittable debe guardar el material.
        material *mat_ptr;
};

/** Calcula la funcion de densidad del punto o+v sabiendo que ha sido generado con una uniforme en la elipse.
   * @param o Punto origen desde el que se genera el rayo hacia la luz.
   * @param v Vector que apunta desde o hacia un punto aleatorio en la elipse.
   * @return Funcion de densidad del punto o+v sabiendo que ha sido generado por una uniforme en la elipse en funcion del area.
   */
float ellipse::pdf_value(const vec3& o, const vec3& v) const {
  hit_record rec;
    if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
      float area = M_PI*axis1.length()*axis2.length();
      float distance_squared = rec.t * rec.t * v.squared_length();
      float cosine = fabs(dot(v, rec.normal) / v.length());
      return distance_squared / (cosine * area);
    }
    else
        return 0;
}

/** Genera un punto aleatorio en la elipse de forma uniforme en funcion del area y devuelve el vector que apunta hacia el desde el observador.
   * @param o Punto origen desde el que se genera el rayo hacia la luz.
   * @return Vector que apunta hacia el punto generado de forma aleatoria.
   */
vec3 ellipse::random(const vec3& o) {
    float rho = random_double(), phi = random_double()*2*M_PI;
    float x = sqrt(rho)*cos(phi);
    float y = sqrt(rho)*sin(phi);
    return center+x*axis1+y*axis2-o;
}

/** Calcula la caja que engloba a la elipse, o bounding box, admitiendo que la elipse se pueda mover con el tiempo.
   * @param t0 Tiempo inicial.
   * @param t1 Tiempo final.
   * @param box Caja que engloba a la elipse, el resultado se devuelve en este parametro.
   * @return Booleano que detecta si la operacion ha podido realizarse o no.
   */
bool ellipse::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(center - axis1 - axis2 -perp*0.0001,center + axis1 + axis2 +perp*0.0001);
    return true;
}

/** Funcion que calcula si un rayo r interseca la elipse en [tmin, tmax]
   * @param r Rayo que podria intersecar la elipse.
   * @param t_min valor inicial del intervalo en el que queremos saber si el rayo interseca a la elipse.
   * @param t_max valor final del intervalo en el que queremos saber si el rayo interseca a la elipse.
   * @return Booleano, es verdadero si interseca y false si no.
   */
bool ellipse::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    float t = (k-r.origin().y()) / r.direction().y();
    if(t < t_min || t > t_max){
      return false;
    }
    else{
      vec3 q = r.origin()+t*r.direction()-center;
      float q_x = dot(q, axis1)/axis1.squared_length();
      float q_y = dot(q, axis2)/axis2.squared_length();
      float r_e = q_x*q_x + q_y*q_y;

      if(r_e <= 1){
        rec.normal = perp;
        rec.p = r.point_at_parameter(t);
        rec.mat_ptr = mat_ptr;
        rec.t = t;
        rec.u = (atan2(q_x, q_y)+M_PI)*1.0/(2*M_PI);
        rec.v = sqrt(r_e);
        return true;
      }
      else{
        return false;
      }
    }
}

#endif
