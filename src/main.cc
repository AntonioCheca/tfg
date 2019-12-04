//==================================================================================================
// Written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is distributed
// without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication along
// with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==================================================================================================

#include "aarect.h"
#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#ifdef _MSC_VER
#include "msc.h"
#endif
#include "pdf.h"
#include "random.h"
#include "sphere.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "surface_texture.h"
#include "texture.h"
#include "rectangleMap.h"
#include "xz_rect_solidangle.h"
#include "ellipses.h"
#include "ellipsessa.h"

#include <float.h>
#include <iostream>
#include <chrono>
#include <string>
#include <fstream>

using namespace std;
#define MAXFLOAT FLT_MAX


inline vec3 de_nan(const vec3& c) {
    vec3 temp = c;
    if (!(temp[0] == temp[0])) temp[0] = 0;
    if (!(temp[1] == temp[1])) temp[1] = 0;
    if (!(temp[2] == temp[2])) temp[2] = 0;
    return temp;
}



vec3 color(const ray& r, hittable *world, hittable *light_shape, int depth) {
    hit_record hrec;
    if (world->hit(r, 0.001, MAXFLOAT, hrec)) {
        scatter_record srec;
        vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
        if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {
            if (srec.is_specular) {
                return srec.attenuation * color(srec.specular_ray, world, light_shape, depth+1);
            }
            else {
                hittable_pdf plight(light_shape, hrec.p);
                mixture_pdf p(&plight, srec.pdf_ptr);
                ray scattered = ray(hrec.p, p.generate(), r.time());
                float pdf_val = p.value(scattered.direction());
                delete srec.pdf_ptr;
                return emitted
                     + srec.attenuation * hrec.mat_ptr->scattering_pdf(r, hrec, scattered)
                                        * color(scattered, world, light_shape, depth+1)
                                        / pdf_val;
            }
        }
        else
            return emitted;
    }
    else
        return vec3(0,0,0);
}

/** Función que crea una caja de cornell con una luz rectangular normal.
  * @param scene Vector de objetos donde se guardará la caja de cornell
  * @param cam Donde se devuelve la cámara que toma la imagen
  * @param aspect Relación de aspecto de la imagen
  */
void cornell_box(hittable **scene, camera **cam, float aspect) {
    int i = 0;
    hittable **list = new hittable*[8];
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new flip_normals(new xz_rect_sa(213, 343, 227, 332, 554, light));
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190),90 , glass);
    list[i++] = new translate(new rotate_y(
                    new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
    *scene = new hittable_list(list,i);
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, vec3(0,1,0),
                      vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

/** Función que crea una caja de cornell con una luz rectangular grande.
  * @param scene Vector de objetos donde se guardará la caja de cornell
  * @param cam Donde se devuelve la cámara que toma la imagen
  * @param aspect Relación de aspecto de la imagen
  */
void cornell_box2(hittable **scene, camera **cam, float aspect) {
    int i = 0;
    hittable **list = new hittable*[7];
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    vec3 p_left_down;
    float x_ax, y_ax, z_ax;
    x_ax = 555;
    y_ax = 555;
    z_ax = 555;
    p_left_down = vec3(0, 0,0);
    list[i++] = new flip_normals(new yz_rect(p_left_down.y(), p_left_down.y()+y_ax, p_left_down.z(), p_left_down.z()+z_ax, p_left_down.x()+x_ax, green));
    list[i++] = new yz_rect(p_left_down.y(), p_left_down.y()+y_ax, p_left_down.z(), p_left_down.z()+z_ax, p_left_down.x(), red);
    //list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
    list[i++] = new flip_normals(new xz_rect(100, 455, 100, 455, 554, light));
    list[i++] = new xz_rect(p_left_down.x(), p_left_down.x()+x_ax, p_left_down.z(), p_left_down.z()+z_ax, p_left_down.y(), white);
    list[i++] = new flip_normals(new xy_rect(p_left_down.x(), p_left_down.x()+x_ax, p_left_down.y(), p_left_down.y()+y_ax, p_left_down.z()+z_ax, white));
    material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190),90 , glass);
    list[i++] = new translate(new rotate_y(
                    new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));

    *scene = new hittable_list(list,i);
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, vec3(0,1,0),
                      vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

/** Función que crea una caja de cornell con objetos representando los cuatro tipos de materiales.
  * @param scene Vector de objetos donde se guardará la caja de cornell
  * @param cam Donde se devuelve la cámara que toma la imagen
  * @param aspect Relación de aspecto de la imagen
  */
void scene_mat(hittable **scene, camera **cam, float aspect) {
    int i = 0;
    hittable **list = new hittable*[9];
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new flip_normals(new xz_rect(213, 343, 227, 332, 554, light));
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(445, 70, 190),70 , glass);
    list[i++] = new sphere(vec3(165, 70, 165),70, new metal(vec3(0.8, 0.8, 0.8), 0.7));
    list[i++] = new sphere(vec3(305, 70, 165),70, green);

    *scene = new hittable_list(list,i);
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, vec3(0,1,0),
                      vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}


/** Función que crea una caja de cornell con una luz de elipse.
  * @param scene Vector de objetos donde se guardará la caja de cornell
  * @param cam Donde se devuelve la cámara que toma la imagen
  * @param aspect Relación de aspecto de la imagen
  */
void cornell_box_ellipse(hittable **scene, camera **cam, float aspect) {
    int i = 0;
    hittable **list = new hittable*[8];
    material *red = new lambertian( new constant_texture(vec3(0.65, 0.05, 0.05)) );
    material *white = new lambertian( new constant_texture(vec3(0.73, 0.73, 0.73)) );
    material *green = new lambertian( new constant_texture(vec3(0.12, 0.45, 0.15)) );
    material *light = new diffuse_light( new constant_texture(vec3(15, 15, 15)) );
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new ellipse(vec3(278, 554, 280), vec3(70,0,0), vec3(0,0,70), light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
    material *glass = new dielectric(1.5);
    list[i++] = new sphere(vec3(190, 90, 190),90 , glass);
    list[i++] = new translate(new rotate_y(
                    new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265,0,295));
    *scene = new hittable_list(list,i);
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
    *cam = new camera(lookfrom, lookat, vec3(0,1,0),
                      vfov, aspect, aperture, dist_to_focus, 0.0, 1.0);
}

int main() {
  // Al main se le han añadido las luces nuevas, en la definición de light_shape, y en lugar de llamar a la función cornell_box se llama a la correspondiente según qué luz queramos. La mayoría del código del main se ha dejado intacto.

  // Hay también comentarios referentes a las órdenes necesarias para crear los batches de imágenes usados para los experimentos. En lugar de imprimir por pantalla el ppm, se imprimía directamente en los ficheros necesarios.

  //ofstream f_c("data_time_ellipse_area_grande.txt");
  //for(int test_n = 1; test_n <= 20; test_n++){

    // Medimos el tiempo que se tarda en crear la imagen
    auto t1 = std::chrono::high_resolution_clock::now();
    //ofstream f("ellipse_area_grande_" + to_string(test_n) + ".ppm");
    int nx = 500;
    int ny = 500;
    int ns = 10;
    cout << "P3\n" << nx << " " << ny << "\n255\n";
    hittable *world;
    camera *cam;
    float aspect = float(ny) / float(nx);

    // El mundo, se utiliza la función dependiendo de qué luz se quiera usar.
    cornell_box_ellipse(&world, &cam, aspect);

    // La luz se define en light_shape, se descomenta la que se quiera usar. Tiene que coincidir con la que se usa en el mundo.

    hittable *light_shape = new ellipse(vec3(278, 554, 280), vec3(70,0,0), vec3(0,0,70), 0);
    //hittable *light_shape = new xz_rect_sa(213, 343, 227, 332, 554, 0);
    //hittable *light_shape = new xz_rect(100, 455, 100, 455, 554, 0);

    hittable *glass_sphere = new sphere(vec3(190, 90, 190),90 , 0);
    hittable *a[2];
    a[0] = light_shape;
    a[1] = glass_sphere;
    hittable_list hlist(a,2);

    for (int j = ny-1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s=0; s < ns; s++) {
                float u = float(i+random_double())/ float(nx);
                float v = float(j+random_double())/ float(ny);
                ray r = cam->get_ray(u, v);
                vec3 p = r.point_at_parameter(2.0);
                col += de_nan(color(r, world, &hlist, 0));
            }
            col /= float(ns);
            col = vec3( sqrt(col[0]), sqrt(col[1]), sqrt(col[2]) );
            int ir = int(255.99*col[0]);
            int ig = int(255.99*col[1]);
            int ib = int(255.99*col[2]);
            ir = clip(ir, 0, 255);
            ig = clip(ig, 0, 255);
            ib = clip(ib, 0, 255);

            cout << ir << " " << ig << " " << ib << "\n";
        }
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();

    cerr << ns << " " << duration << endl;
    //f.close();
  //}
  //f_c.close();
}
