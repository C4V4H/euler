#ifndef GEOMETRY_H 
#define GEOMETRY_H

#include <math.h>
#include <stddef.h>
#include <stdlib.h>

/* ===================== STRUCT DEFINITION ===================== */

typedef struct {
    double x, y;
} vec2;

typedef struct {
    size_t x, y;
} vec2u;

typedef struct {
    double x, y, z;
} vec3;

typedef struct {
    double x, y, z, w;
} quat;


/* ========================= DEFINES ========================= */

#define _GET_1ST_ARG(x, ...) x

/* Sum Macros */
#define _SUM_V2(...) \
    vec2_sum_array(sizeof((vec2[]){__VA_ARGS__})/sizeof(vec2), (vec2[]){__VA_ARGS__})
#define _SUM_V3(...) \
    vec3_sum_array(sizeof((vec3[]){__VA_ARGS__})/sizeof(vec3), (vec3[]){__VA_ARGS__})

#define vec_sum(...) \
    _Generic( _GET_1ST_ARG(__VA_ARGS__), \
        vec2: _SUM_V2(__VA_ARGS__),      \
        vec3: _SUM_V3(__VA_ARGS__)       \
    )

/* Sub Macros */
#define _SUB_V2(...) \
    vec2_sub_array(sizeof((vec2[]){__VA_ARGS__})/sizeof(vec2), (vec2[]){__VA_ARGS__})
#define _SUB_V3(...) \
    vec3_sub_array(sizeof((vec3[]){__VA_ARGS__})/sizeof(vec3), (vec3[]){__VA_ARGS__})

#define vec_sub(...) \
    _Generic( _GET_1ST_ARG(__VA_ARGS__), \
        vec2: _SUB_V2(__VA_ARGS__),      \
        vec3: _SUB_V3(__VA_ARGS__)       \
    )

/* ===================== BASIC OPERATIONS ===================== */

static inline double
vec2_len(vec2 v) {
    return sqrt(v.x*v.x + v.y*v.y);
}

static inline double
vec3_len(vec3 v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

static inline vec2
vec2_add(vec2 v1, vec2 v2) {
    return (vec2) { v1.x + v2.x, v1.y + v2.y };
}

static inline vec3
vec3_add(vec3 v1, vec3 v2) {
    return (vec3) { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

static inline vec2
vec2_sub(vec2 v1, vec2 v2) {
    return (vec2) { v1.x - v2.x, v1.y - v2.y };
}

static inline vec3
vec3_sub(vec3 v1, vec3 v2) {
    return (vec3) { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

static inline vec2
vec2_scale(vec2 v, double s) {
    return (vec2) { v.x*s, v.y*s };
}

static inline vec3
vec3_scale(vec3 v, double s) {
    return (vec3) { v.x*s, v.y*s, v.z*s };
}


/* ===================== ARRAY OPERATIONS ===================== */

static inline vec2
vec2_sum_array(int count, const vec2* vecs) {
    vec2 res = {0};
    for (int i = 0; i < count; i++) {
        res.x += vecs[i].x;
        res.y += vecs[i].y;
    }
    return res;
}

static inline vec3
vec3_sum_array(int count, const vec3* vecs) {
    vec3 sum = {0, 0, 0};
    for (int i = 0; i < count; i++) {
        sum.x += vecs[i].x;
        sum.y += vecs[i].y;
        sum.z += vecs[i].z;
    }
    return sum;
}

static inline vec2
vec2_sub_array(int count, const vec2* vecs) {
    if (count <= 0) return (vec2){0, 0};
    vec2 res = vecs[0];
    for (int i = 1; i < count; i++) {
        res.x -= vecs[i].x;
        res.y -= vecs[i].y;
    }
    return res;
}

static inline vec3
vec3_sub_array(int count, const vec3* vecs) {
    if (count <= 0) return (vec3){0, 0, 0};
    vec3 res = vecs[0];
    for (int i = 1; i < count; i++) {
        res.x -= vecs[i].x;
        res.y -= vecs[i].y;
        res.z -= vecs[i].z;
    }
    return res;
}

/* ===================== DOT & CROSS PRODUCT ===================== */

static inline double
vec2_dot_product(vec2 v1, vec2 v2) {
    return v1.x*v2.x + v1.y*v2.y;
}

static inline double
vec3_dot_product(vec3 v1, vec3 v2) {
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

static inline vec3
vec3_cross_product(vec3 v1, vec3 v2) {
    return (vec3) {
        v1.y*v2.z - v1.z*v2.y,
        v1.z*v2.x - v1.x*v2.z,
        v1.x*v2.y - v1.y*v2.x
    };
}

/* ===================== NORMALIZATION & PROJECT ===================== */

static inline vec2
vec3_project(vec3 v) {
    if (v.z == 0) v.z = 0.00001f;
    return (vec2) { v.x/v.z, v.y/v.z };
}

static inline vec2
vec2_normalized(vec2 v) {
    double len = vec2_len(v);
    if (len == 0) return (vec2) {0, 0};
    return (vec2){ v.x / len, v.y / len };
}

static inline vec3
vec3_normalized(vec3 v) {
    double len = vec3_len(v);
    if (len == 0) return (vec3) {0, 0, 0};
    return (vec3){ v.x / len, v.y / len, v.z / len };
}

static inline vec3
vec3_surface_normal(vec3 a, vec3 b, vec3 c) {
    vec3 u = vec3_sub(b, a);
    vec3 v = vec3_sub(c, a);
    vec3 n = vec3_cross_product(u, v);
    return vec3_normalized(n);
}

/* ===================== ROTATIONS (BASIC) ===================== */

static inline vec2
vec2_rotated(vec2 v, double angle_rad) {
    double c = cos(angle_rad);
    double s = sin(angle_rad);
    return (vec2) { v.x * c - v.y * s, v.x * s + v.y * c };
}

static inline vec3
vec3_rotated_x(vec3 v, double angle_rad) {
    double c = cos(angle_rad);
    double s = sin(angle_rad);
    return (vec3) { v.x, v.y * c - v.z * s, v.y * s + v.z * c };
} 

static inline vec3
vec3_rotated_y(vec3 v, double angle_rad) {
    double c = cos(angle_rad);
    double s = sin(angle_rad);
    return (vec3) { v.x * c - v.z * s, v.y, v.x * s + v.z * c };
} 

static inline vec3
vec3_rotated_z(vec3 v, double angle_rad) {
    double c = cos(angle_rad);
    double s = sin(angle_rad);
    return (vec3) { v.x * c - v.y * s, v.x * s + v.y * c, v.z };
}

/* ===================== QUATERNIONS ===================== */

static inline quat
quat_mul(quat a, quat b) {
    return (quat) {
        .x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
        .y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
        .z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
        .w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
    };
}

static inline quat
quat_from_axis_angle(vec3 axis, double angle_rad) {
    double half_angle = angle_rad / 2.0;
    double s = sin(half_angle);
    vec3 n = vec3_normalized(axis);
    
    return (quat) {
        .x = n.x * s, .y = n.y * s, .z = n.z * s,
        .w = cos(half_angle)
    };
}

static inline quat
quat_from_euler(vec3 angles) {
    quat qx = quat_from_axis_angle((vec3){1,0,0}, angles.x);
    quat qy = quat_from_axis_angle((vec3){0,1,0}, angles.y);
    quat qz = quat_from_axis_angle((vec3){0,0,1}, angles.z);

    // Ordine Z * Y * X
    quat q_res = quat_mul(qz, qy);
    q_res = quat_mul(q_res, qx);
    return q_res;
}

static inline vec3
vec3_rotate_by_quat(vec3 v, quat q) {
    vec3 u = {q.x, q.y, q.z};
    double s = q.w;

    vec3 uv = vec3_cross_product(u, v);
    vec3 uuv = vec3_cross_product(u, uv);
    
    // Formula: v + 2*s*uv + 2*uuv
    uv = vec3_scale(uv, 2.0 * s);
    uuv = vec3_scale(uuv, 2.0);

    return vec3_add(v, vec3_add(uv, uuv));
}

/* ===================== MAIN ROTATION ===================== */

/* Rotates the vector `v` by the given `angles` vector
 * for single vectors (such as {1,0,0}) it will use the rotation matrix
 * otherwise it will use the quaterion method using the Z-Y-X rotation order
 */
static inline vec3
vec3_rotated(vec3 v, vec3 angles) {
    if (angles.x == 0 && angles.y == 0 && angles.z == 0) return v;
    if (angles.y == 0 && angles.z == 0) return vec3_rotated_x(v, angles.x);
    if (angles.x == 0 && angles.z == 0) return vec3_rotated_y(v, angles.y);
    if (angles.x == 0 && angles.y == 0) return vec3_rotated_z(v, angles.z);

    quat q = quat_from_euler(angles);
    return vec3_rotate_by_quat(v, q);
}

#endif // GEOMETRY_H
