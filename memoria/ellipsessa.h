#ifndef ELLIPSESSAH
#define ELLIPSESSAH

#include <cmath>
#include "hittable.h"
#include "onb.h"
#include "pdf.h"
#include "random.h"

using namespace std;
/// Funcion para eliminar NaNs
inline vec3 de_nan1(const vec3& c) {
    vec3 temp = c;
    if (!(temp[0] == temp[0])) temp[0] = 0;
    if (!(temp[1] == temp[1])) temp[1] = 0;
    if (!(temp[2] == temp[2])) temp[2] = 0;
    return temp;
}
/// Algoritmo de Simpson compuesto para aproximar integrales de funciones.
float composite_simpson(float f(float, float, float), float a, float b, int n, float a_t, float b_t){
  float step = (b-a)/n;
  float s1 = f(a+step/2, a_t, b_t);
  float s2 = 0;

  for(int i = 1; i < n; i++){
    s1 += f(a+step*i+step/2, a_t, b_t);
    s2 += f(a+step*i, a_t, b_t);
  }
  float f_a = f(a, a_t, b_t);
  float f_b = f(b, a_t, b_t);
  return (step/6) * (f_a+f_b+4*s1+2*s2);
}

/// Funcion h_p del paper de Guillen et al. 2017
float h_p(float phi_p, float a_t, float b_t){
  float a_ts = a_t*a_t;
  float b_ts = b_t*b_t;
  float c_t = a_t/(sqrt(1+a_ts));
  float p = 1.0/(b_ts);
  float m = (a_ts-b_ts)/(a_ts+1);
  float s_p = sin(phi_p);
  s_p = s_p*s_p;

  float disc = (1-(p+1)*s_p)/(1-(m*p+1)*s_p);
  if(disc >= 0)
    return c_t*sqrt(disc);
  else
    return 0;
}

/// El doble de la funcion h_p, que es el integrando que aparece en la definicion de Omega_p
float double_h_p(float phi_p, float a_t, float b_t){
  return 2.0*h_p(phi_p, a_t, b_t);
}
/// Parametro que indica cuantas separaciones realizamos en Simpson compuesto
int precision_int = 10;

/// Funcion Omega_p del paper de Guillen et al. 2017. La aproximamos con Simpson compuesto.
float omega_p(float phi_p, float a_t, float b_t, float beta){
  if(phi_p >= 0){
    return composite_simpson(double_h_p, 0, beta, precision_int, a_t, b_t)+composite_simpson(double_h_p, 0, phi_p, precision_int, a_t, b_t);
  }
  else{
    return composite_simpson(double_h_p, 0, beta, precision_int, a_t, b_t)-composite_simpson(double_h_p, 0, -phi_p, precision_int, a_t, b_t);
  }
}

/// Struct que genera la ecuacion a resolver, con la que se define Phi en el paper de Guillen et al. 2017
struct EcuacionPhi{
  float a_t, b_t, beta, k;
  void setParameters(float a, float b, float bet, float k2){
    a_t = a;
    b_t = b;
    beta = bet;
    k = k2;
  }
  float g(float x){
    return omega_p(x, a_t, b_t, beta)-k;
  }
};

/// Algoritmo de biseccion para encontrar raices de funciones
float bisection(EcuacionPhi& f, double a, double b, int n_max, float tol){
  float f_a = f.g(a), f_b = f.g(b);
  if(f_a*f_b >= 0){
    return 0;
  }
  else{
    float c = (a+b)/2;
    int n = 1;
    float f_c = f.g(c);
    while(n <= n_max && (b-a)/2 >= tol && f_c != 0){
      c = (a+b)/2;
      f_c = f.g(c);
      n++;
      if(f_c*f_a >= 0){
        a = c;
      }
      else
        b = c;
    }
    return c;
  }
}

/** Clase elipse_sa, subclase de hittable, que representa el modelo de un disco con generacion de puntos en funcion del angulo solido. Esta generacion es uniforme en la elipse esferica que produce el disco al proyectarse en una esfera de radio 1. Se diferencia de la clase ellipse en las funciones pdf_value y random.
  */
class ellipse_sa: public hittable  {
    public:
        ellipse_sa() {}
        /// El constructor. Necesita un centro, los dos ejes y el material.
        ellipse_sa(vec3 cen, vec3 ax1, vec3 ax2, material *m) : center(cen), axis1(ax1), axis2(ax2), mat_ptr(m)  {calcPerp();
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
        /// Genera la funcion de distribucion del punto elegido con random. Es en funcion del angulo solido asi que es constante.
        virtual float  pdf_value(const vec3& o, const vec3& v) const;
        /// Genera un vector desde el punto o hacia un punto escogido de forma uniforme en la elipse respecto al angulo solido.
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
        /// area de la elipse esferica, el disco proyectado.
        float area_omega;
        /// Los extremos de la caja que engloba a la elipse.
        float x0, x1, z0, z1, k;
        /// Material de la elipse, todo hittable debe guardar el material.
        material *mat_ptr;
};

/** Calcula la funcion de densidad del punto o+v sabiendo que ha sido generado con una uniforme respecto al angulo solido.
   * @param o Punto origen desde el que se genera el rayo hacia la luz.
   * @param v Vector que apunta desde o hacia un punto aleatorio en la elipse.
   * @return Funcion de densidad del punto o+v sabiendo que ha sido generado por una uniforme en la elipse en funcion del angulo solido.
   */
float ellipse_sa::pdf_value(const vec3& o, const vec3& v) const {
  hit_record rec;
    if (this->hit(ray(o, v), 0.001, FLT_MAX, rec)) {
      return 1.0/area_omega;
    }
    else
        return 0;
}
/// Parametro que indica cuantas iteraciones ejecutamos biseccion.
int n_it_bisection = 10;


/** Genera un punto aleatorio en la elipse de forma uniforme en funcion del angulo solido y devuelve el vector que apunta hacia el desde el observador.
   * @param o Punto origen desde el que se genera el rayo hacia la luz.
   * @return Vector que apunta hacia el punto generado de forma aleatoria.
   */
vec3 ellipse_sa::random(const vec3& o) {
    float rho = random_double(), phi = random_double()*2*M_PI;
    float x = sqrt(rho)*cos(phi);
    float y = sqrt(rho)*sin(phi);

    float e_1 = random_double(), e_2 = random_double();
    vec3 z_d = -cross(axis1, axis2);
    z_d.make_unit_vector();

    vec3 x_d = cross(z_d, center-o);
    x_d.make_unit_vector();
    vec3 y_d = cross(z_d, x_d);
    vec3 y1 = center+y_d*axis1.length();
    vec3 y0 = center-y_d*axis1.length();
    vec3 y1_proy = y1-o, y0_proy = y0-o;
    y1_proy.make_unit_vector();
    y0_proy.make_unit_vector();
    float yp_1 = dot(y1_proy, y_d), yp_0 = dot(y0_proy, y_d);
    float zp_1 = dot(y1_proy, z_d), zp_0 = dot(y0_proy, z_d);
    vec3 z_e = (yp_0+yp_1)*y_d/2.0 + (zp_0+zp_1)*z_d/2.0;
    z_e.make_unit_vector();
    vec3 z_e_disk = z_e * (dot(center-o, z_d)/dot(z_e, z_d))+o;

    float x_cos = sqrt(axis1.squared_length()-dot(z_e_disk-center, y_d)*dot(z_e_disk-center, y_d));

    vec3 x1 = z_e_disk+x_d*x_cos, x0 = z_e_disk-x_d*x_cos;
    vec3 x1_proy = x1-o, x0_proy = x0-o;
    x1_proy.make_unit_vector();
    x0_proy.make_unit_vector();
    float xp_1 = dot(x1_proy, x_d), xp_0 = dot(x0_proy, x_d);
    float a = xp_1, b = sqrt((yp_1-yp_0)*(yp_1-yp_0)+(zp_1-zp_0)*(zp_1-zp_0))/2.0;
    float alpha = asin(a);
    float beta = asin(b);
    float a_t = tan(alpha);
    float b_t = tan(beta);

    vec3 x_e = x_d, y_e = cross(z_e, x_e);
    float Omega_D = omega_p(beta, a_t, b_t, beta);
    EcuacionPhi ec;
    ec.setParameters(a_t, b_t, beta, e_1*Omega_D);
    float phi_p = bisection(ec, -beta, beta, n_it_bisection, 0.001);
    float h = (2*e_2-1)*h_p(phi_p, a_t, b_t);
    float sq = sqrt(1-h*h);
    vec3 q(h, sq*sin(phi_p), sq*cos(phi_p));
    vec3 q2 = h*x_e + sq*sin(phi_p)*y_e + sq*cos(phi_p)*z_e;
    area_omega = Omega_D;
    return q2;
}

/** Calcula la caja que engloba a la elipse, o bounding box, admitiendo que la elipse se pueda mover con el tiempo.
   * @param t0 Tiempo inicial.
   * @param t1 Tiempo final.
   * @param box Caja que engloba a la elipse, el resultado se devuelve en este parametro.
   * @return Booleano que detecta si la operacion ha podido realizarse o no.
   */
bool ellipse_sa::bounding_box(float t0, float t1, aabb& box) const {
    box = aabb(center - axis1 - axis2 -perp*0.0001,center + axis1 + axis2 +perp*0.0001);
    return true;
}

/** Funcion que calcula si un rayo r interseca la elipse en [tmin, tmax]
   * @param r Rayo que podria intersecar la elipse.
   * @param t_min valor inicial del intervalo en el que queremos saber si el rayo interseca a la elipse.
   * @param t_max valor final del intervalo en el que queremos saber si el rayo interseca a la elipse.
   * @return Booleano, es verdadero si interseca y false si no.
   */
bool ellipse_sa::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    float t = dot(-r.origin()+center, perp)*1.0/dot(r.direction(), perp);
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
