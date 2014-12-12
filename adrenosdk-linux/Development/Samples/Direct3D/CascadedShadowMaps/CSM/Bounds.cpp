//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include "FrmPlatform.h"
#include "Bounds.h"

/*
 */
#define BOUNDS_EPSILON	1e-4f

/******************************************************************************\
*
* BoundSphere
*
\******************************************************************************/

/*
 */
BoundSphere::BoundSphere() : valid(0) {
    
}

BoundSphere::BoundSphere(  BoundBox &bb) {
    set(bb);
}

BoundSphere::~BoundSphere() {
    
}

/*
 */
void BoundSphere::init() {
    valid = 0;
    center = FRMVECTOR3( 0.0f, 0.0f, 0.0f);
    radius = -1.0f;
}

void BoundSphere::set(  BoundBox &bb) {
    init();
    expand(bb);
}
void BoundSphere::set(FRMVECTOR3 &cent, float rad) {
    center = cent;
    radius = rad;
}

/*
 */
void BoundSphere::expand(FRMVECTOR3 &point) {
    if(isValid()) {
        FRMVECTOR3 dir = point - center;
        float len = FrmVector3Length(dir);
        if(len > radius) {
            float delta = (len - radius) * 0.5f;
            center += dir * (delta / len);
            radius += delta;
        }
    } else {
        valid = 1;
        center = point;
        radius = BOUNDS_EPSILON;
    }
}

void BoundSphere::expand(  BoundBox &bb) {
    if(bb.isValid()) {
        FRMVECTOR3 &min = bb.getMin();
        FRMVECTOR3 &max = bb.getMax();
        if(isValid()) {
            expand(FRMVECTOR3(min.x,min.y,min.z));
            expand(FRMVECTOR3(max.x,min.y,min.z));
            expand(FRMVECTOR3(min.x,max.y,min.z));
            expand(FRMVECTOR3(max.x,max.y,min.z));
            expand(FRMVECTOR3(min.x,min.y,max.z));
            expand(FRMVECTOR3(max.x,min.y,max.z));
            expand(FRMVECTOR3(min.x,max.y,max.z));
            expand(FRMVECTOR3(max.x,max.y,max.z));
        } else {
            valid = 1;
            center = (min + max) * 0.5f;
            radius = FrmVector3Length(max - center);
        }
    }
}
/******************************************************************************\
*
* BoundBox
*
\******************************************************************************/

/*
 */
BoundBox::BoundBox() : valid(0) {
    
}

BoundBox::~BoundBox() {
    
}


void BoundBox::set(FRMVECTOR3 &min_, FRMVECTOR3 &max_) {
    min = min_;
    max = max_;
    valid = (min.x <= max.x && min.y <= max.y && min.z <= max.z);
}

/*
 */
void BoundBox::expand(FRMVECTOR3 &point) {
    if(isValid()) {
        if(min.x > point.x) min.x = point.x;
        if(max.x < point.x) max.x = point.x;
        if(min.y > point.y) min.y = point.y;
        if(max.y < point.y) max.y = point.y;
        if(min.z > point.z) min.z = point.z;
        if(max.z < point.z) max.z = point.z;
    } else {
        valid = 1;
        min = point - FRMVECTOR3(BOUNDS_EPSILON, BOUNDS_EPSILON, BOUNDS_EPSILON);
        max = point + FRMVECTOR3(BOUNDS_EPSILON, BOUNDS_EPSILON, BOUNDS_EPSILON);
    }
}
