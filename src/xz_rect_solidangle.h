#ifndef _SOLID_ANGLE_RECT__
#define _SOLID_ANGLE_RECT__


#include "hittable.h"
#include "random.h"
#include "rectangleMap.h"

/** Clase rectángulo, subclase de hittable, que representa el modelo de un rectángulo paralelo al plano XZ con capacidad de generar puntos aleatorios uniformemente en función del ángulo sólido.
  */
class xz_rect_sa: public hittable  {
    public:
        SphQuad squad;
        material  *mp;
        float x0, x1, z0, z1, k;
        xz_rect_sa() {}
        /// El constructor. Necesita las cuatro esquinas, la altura k y el material.
        xz_rect_sa(float _x0, float _x1, float _z0, float _z1, float _k, material *mat) : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};
        /// Función que calcula si un rayo r interseca con el rectángulo en [t0, t1], y guarda los datos de intersección en el hit_record rec.
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        /// Calcula la caja que engloba al rectángulo. Como es dos dimensional, la caja añade "altura" en el eje en el que el rectángulo es plano.
        virtual bool bounding_box(float t0, float t1, aabb& box) const {
            box =  aabb(vec3(x0,k-0.0001,z0), vec3(x1, k+0.0001, z1));
            return true;
        }
        /// Genera la función de densidad del punto elegido con random. Es en función del ángulo sólido, así que es constante.
        virtual float  pdf_value(const vec3& o, const vec3& v) const {
            hit_record rec;
            if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
                return (1.0/squad.S);
            }
            else
                return 0;
        }
        /// Genera un vector desde el punto o hacia un punto escogido de forma uniforme en el rectángulo en función del ángulo sólido.
        virtual vec3 random(const vec3& o) {
          SphQuadInit(squad, vec3(x0,k,z0), vec3(x1-x0,0,0), vec3(0,0,z1-z0), o);
          float u = random_double(), v = random_double();
          squad.p = vec3(x0 + (x1-x0)*u, k, z0+(z1-z0)*v);
          vec3 random_v = SphQuadSample(squad, u, v);
          return random_v-o;
        }
};

/** Función que calcula si un rayo r interseca al rectángulo en [t0, t1]
   * @param r Rayo que podría intersecar al rectángulo.
   * @param t0 valor inicial del intervalo en el que queremos saber si el rayo interseca al rectángulo.
   * @param t1 valor final del intervalo en el que queremos saber si el rayo interseca al rectángulo.
   * @return Booleano, es verdadero si interseca y false si no.
   */
bool xz_rect_sa::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    float t = (k-r.origin().y()) / r.direction().y();
    if (t < t0 || t > t1)
        return false;
    float x = r.origin().x() + t*r.direction().x();
    float z = r.origin().z() + t*r.direction().z();
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;
    rec.u = (x-x0)/(x1-x0);
    rec.v = (z-z0)/(z1-z0);
    rec.t = t;
    rec.mat_ptr = mp;
    rec.p = r.point_at_parameter(t);
    rec.normal = vec3(0, 1, 0);
    return true;
}

#endif
