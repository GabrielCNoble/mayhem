#ifndef PHY_COL_H
#define PHY_COL_H


#include "phy_common.h"


int phy_ContactBoxTriangle(struct collision_triangle_t *triangle, struct box_shape_t *box_shape, vec3_t &position, mat3_t &orientation, struct contact_point_t *contact);

//int phy_ContactCapsuleTriangle(struct collision_triangle_t *triangle, struct capsule_shape_t *capsule_shape, vec3_t &position, struct contact_point_t *contact);

int phy_ContactCapsuleTriangle(struct collision_triangle_t *triangle, struct capsule_shape_t *capsule_shape, vec3_t &position, vec3_t &velocity, struct contact_point_t *contact);

int phy_ContactBoxBox(struct box_shape_t *box_shape_a, vec3_t &position_a, struct box_shape_t *box_shape_b, vec3_t &position_b, struct contact_point_t *contact);

int phy_ContactCapsuleBox(struct box_shape_t *box_shape, vec3_t &position_box, struct capsule_shape_t *capsule, vec3_t &position_capsule, struct contact_point_t *contact);


#endif // PHY_COL_H
