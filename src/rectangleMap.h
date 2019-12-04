#ifndef _RECTANGLE_AREA_MAP__
#define _RECTANGLE_AREA_MAP__

#define PI_R_MAP 3.14159265

#include <cmath>
using namespace std;

/// Devuelve el número más cercano a n de un intervalo. Es la identidad si el número está en el intervalo, o el extremo más cercano si no está.
float clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

/// Estructura que guarda parámetros para los cálculos de la proyección del rectángulo en la esfera
struct SphQuad{
  vec3 o, x, y, z, p;
  float z0, z0sq;
  float x0, y0, y0sq;
  float x1, y1, y1sq;
  float b0, b1, b0sq, k;
  float S;
};

/// Inicializa los parámetros de SphQuad, necesarios para los cálculos posteriores y que inicializan la proyección del rectángulo en la esfera.
void SphQuadInit(SphQuad& squad, vec3 s, vec3 ex, vec3 ey, vec3 o){
  squad.o = o;
  float exl = ex.length(), eyl = ey.length();

  squad.x = ex/exl;
  squad.y = ey/eyl;
  squad.z = cross(squad.x, squad.y);

  vec3 d = s-o;
  squad.z0 = dot(d, squad.z);

  if(squad.z0 > 0){
    squad.z *= -1;
    squad.z0 *= -1;
  }
  squad.z0sq = squad.z0 * squad.z0;
  squad.x0 = dot(d, squad.x);
  squad.y0 = dot(d, squad.y);
  squad.x1 = squad.x0 + exl;
  squad.y1 = squad.y0 + eyl;
  squad.y0sq = squad.y0*squad.y0;
  squad.y1sq = squad.y1*squad.y1;

  vec3 v00 = vec3(squad.x0, squad.y0, squad.z0);
  vec3 v01 = vec3(squad.x0, squad.y1, squad.z0);
  vec3 v10 = vec3(squad.x1, squad.y0, squad.z0);
  vec3 v11 = vec3(squad.x1, squad.y1, squad.z0);

  vec3 n0 = unit_vector(cross(v00, v10));
  vec3 n1 = unit_vector(cross(v10, v11));
  vec3 n2 = unit_vector(cross(v11, v01));
  vec3 n3 = unit_vector(cross(v01, v00));

  float g0 = acos(-dot(n0,n1));
  float g1 = acos(-dot(n1,n2));
  float g2 = acos(-dot(n2,n3));
  float g3 = acos(-dot(n3, n0));

  squad.b0 = n0.z();
  squad.b1 = n2.z();
  squad.b0sq = squad.b0 * squad.b0;
  squad.k = 2*PI_R_MAP-g2-g3;
  squad.S = g0+g1-squad.k;
}

/// Genera un punto aleatorio en la proyección del rectángulo de forma uniforme respecto al ángulo sólido
vec3 SphQuadSample(SphQuad& squad, float u, float v){
  float au = u*squad.S + squad.k;
  float fu = (cos(au) * squad.b0 - squad.b1)*1.0/sin(au);
  float cu = 1.0/sqrt(fu*fu+squad.b0sq) * (fu > 0 ? +1 : -1);
  cu = clip(cu, -1, 1);
  float xu = -(cu*squad.z0)*1.0/sqrt(1-cu*cu);
  xu = clip(xu, squad.x0, squad.x1);
  float d = sqrt(xu*xu + squad.z0sq);
  float h0 = squad.y0/sqrt(d*d+squad.y0sq);
  float h1 = squad.y1 / sqrt(d*d + squad.y1sq);
  float hv = h0 + v*(h1-h0);
  float hv2 = hv*hv;
  float yv = (hv2 < 1-0.0001) ? (hv*d)/sqrt(1-hv2) : squad.y1;

  return (squad.o + xu*squad.x + yv*squad.y + squad.z0*squad.z);
}

#endif
