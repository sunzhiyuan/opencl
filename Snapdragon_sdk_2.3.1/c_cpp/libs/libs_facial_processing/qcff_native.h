/* ======================================================================
 *  Copyright © 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 *  QTI Proprietary and Confidential.
 *  =====================================================================
 * @file    qcff_native.h
 *
 */

#ifndef QCFF_H
#define QCFF_H

#include <stdint.h>

/* MACROS */
#define QCFF_SUCCEEDED(x)   (QCFF_RET_SUCCESS == x)
#define QCFF_FAILED(x)      (QCFF_RET_SUCCESS != x)

//================================================================
//   Enumerations
//================================================================

/* Return result for the Common Interface API calls */
#define   QCFF_RET_SUCCESS         0
#define   QCFF_RET_FAILURE         1
#define   QCFF_RET_NO_RESOURCE     2
#define   QCFF_RET_INVALID_PARM    3
#define   QCFF_RET_NO_MATCH        4

/* Features supported */
typedef enum
{
	QCFF_FACIAL_PROCESSING = 1,
	QCFF_FACIAL_RECOGNITION,
}qcff_feature;


/* The still/video mode */
typedef enum
{
    QCFF_MODE_VIDEO = 0, // default
    QCFF_MODE_STILL = 1,
    QCFF_MODE_MAX,
} qcff_mode_t;

typedef enum {
    QCFF_ENGINE_FACE_ENGINE = 0,
    QCFF_ENGINE_M21,
    QCFF_ENGINE_MAX
} qcff_engine_t;

/* The facial parts locations */
typedef enum
{
    QCFF_PARTS_LEFT_EYE_CENTER = 0,    /* Center of left eye        */
    QCFF_PARTS_RIGHT_EYE_CENTER,       /* Center of right eye       */
    QCFF_PARTS_MOUTH_CENTER,           /* Center of mouth           */
    QCFF_PARTS_LEFT_EYE_INNER,         /* Inner corner of left eye  */
    QCFF_PARTS_LEFT_EYE_OUTER,         /* Outer corner of left eye  */
    QCFF_PARTS_RIGHT_EYE_INNER,        /* Inner corner of right eye */
    QCFF_PARTS_RIGHT_EYE_OUTER,        /* Outer corner of right eye */
    QCFF_PARTS_MOUTH_LEFT,             /* Left corner of mouth      */
    QCFF_PARTS_MOUTH_RIGHT,            /* Right corner of mouth     */
    QCFF_PARTS_NOSTRIL_LEFT,           /* Left nostril              */
    QCFF_PARTS_NOSTRIL_RIGHT,          /* Right nostril             */
    QCFF_PARTS_MOUTH_UP,               /* Mouth up                  */
    QCFF_PARTS_MAX
} qcff_simple_parts_idx_t;

/* The facial parts ex */
typedef enum
{
    QCFF_PARTS_EX_EYE_L_PUPIL = 0,   /* Left Eye Pupil        */
    QCFF_PARTS_EX_EYE_L_IN,          /* Left Eye Pupil Inner  */
    QCFF_PARTS_EX_EYE_L_OUT,         /* Left Eye Pupil Outer  */
    QCFF_PARTS_EX_EYE_L_UP,          /* Left Eye Pupil Upper  */
    QCFF_PARTS_EX_EYE_L_DOWN,        /* Left Eye Pupil Lower  */
    QCFF_PARTS_EX_EYE_R_PUPIL,       /* Right Eye Pupil       */
    QCFF_PARTS_EX_EYE_R_IN,          /* Right Eye Pupil Inner */
    QCFF_PARTS_EX_EYE_R_OUT,         /* Right Eye Pupil Outer */
    QCFF_PARTS_EX_EYE_R_UP,          /* Right Eye Pupil Upper */
    QCFF_PARTS_EX_EYE_R_DOWN,        /* Right Eye Pupil Lower */
    QCFF_PARTS_EX_FOREHEAD,          /* Forehead              */
    QCFF_PARTS_EX_NOSE,              /* Nose                  */
    QCFF_PARTS_EX_NOSE_TIP,          /* Nose Tip              */
    QCFF_PARTS_EX_NOSE_L,            /* Nose Lower Left       */
    QCFF_PARTS_EX_NOSE_R,            /* Nose Lower Right      */
    QCFF_PARTS_EX_NOSE_L_0,          /* Nose Middle Left      */
    QCFF_PARTS_EX_NOSE_R_0,          /* Nose Middle Right     */
    QCFF_PARTS_EX_NOSE_L_1,          /* Nose Upper Left       */
    QCFF_PARTS_EX_NOSE_R_1,          /* Nose Upper Right      */
    QCFF_PARTS_EX_MOUTH_L,           /* Mouth Left            */
    QCFF_PARTS_EX_MOUTH_R,           /* Mouth Right           */
    QCFF_PARTS_EX_MOUTH_UP,          /* Mouth Up              */
    QCFF_PARTS_EX_MOUTH_DOWN,        /* Mouth Down            */
    QCFF_PARTS_EX_LIP_UP,            /* Upper Lip             */
    QCFF_PARTS_EX_LIP_DOWN,          /* Lower Lip             */
    QCFF_PARTS_EX_BROW_L_UP,         /* Left Eyebrow Up       */
    QCFF_PARTS_EX_BROW_L_DOWN,       /* Left Eyebrow Down     */
    QCFF_PARTS_EX_BROW_L_IN,         /* Left Eyebrow Inner    */
    QCFF_PARTS_EX_BROW_L_OUT,        /* Left Eyebrow Outer    */
    QCFF_PARTS_EX_BROW_R_UP,         /* Right Eyebrow Up      */
    QCFF_PARTS_EX_BROW_R_DOWN,       /* Right Eyebrow Down    */
    QCFF_PARTS_EX_BROW_R_IN,         /* Right Eyebrow Inner   */
    QCFF_PARTS_EX_BROW_R_OUT,        /* Right Eyebrow Outer   */
    QCFF_PARTS_EX_CHIN,              /* Chin                  */
    QCFF_PARTS_EX_CHIN_L,            /* Chin Left             */
    QCFF_PARTS_EX_CHIN_R,            /* Chin Right            */
    QCFF_PARTS_EX_EAR_L_DOWN,        /* Ear Left Lower        */
    QCFF_PARTS_EX_EAR_R_DOWN,        /* Ear Right Lower       */
    QCFF_PARTS_EX_EAR_L_UP,          /* Ear Left Upper        */
    QCFF_PARTS_EX_EAR_R_UP,          /* Ear Right Upper       */
    QCFF_PARTS_EX_MAX
} qcff_parts_ex_idx_t;

/* QCFF Configuration */
typedef struct {
    uint32_t             width;     /* Frame Width */
    uint32_t             height;    /* Frame Height */

    /* experimental feature */
    uint32_t             downscale_factor; /* downscale the image before processing */
} qcff_config_t;

/* Location struction */
typedef struct {
    int32_t              x;
    int32_t              y;
} qcff_pixel_t;

/* Facial parts structure (simple)
   Refer to the qcff_simple_parts_idx_t enum
   for mapping between parts index and the
   actual facial parts location */
typedef struct {
    qcff_pixel_t         parts[QCFF_PARTS_MAX];
} qcff_face_parts_t;

/* Facial parts structure (detailed)
   Please refer to documentataion for the
   graphical representation of the detailed
   facial parts location */
typedef struct {
    qcff_pixel_t         parts[QCFF_PARTS_EX_MAX];
} qcff_face_parts_ex_t;

/* Face rectangle
   Coordinates with respect to the input frame */
typedef struct {
    uint32_t             x;
    uint32_t             y;
    uint32_t             dx;
    uint32_t             dy;
} qcff_rect_t;

typedef struct {
    /* Four corners of the face relative to the facial features: top left, top right,
    * bottom left and bottom right. */
    qcff_pixel_t         top_left;
    qcff_pixel_t         top_right;
    qcff_pixel_t         bottom_left;
    qcff_pixel_t         bottom_right;
    /* Smallest rectangular box bounding the four corners */
    qcff_rect_t          bounding_box;
} qcff_face_rect_t;

/* Eye open-close degree in the range of 0-100 */
typedef struct {
    int32_t              left;
    int32_t              right;
} qcff_eye_open_deg_t;

/* Gaze degree along the two planes: left-right and up-down
   (right: +ve, left: -ve, up: +ve, down: -ve)
   The degree ranges from -90 to 90 */
typedef struct {
    int32_t              left_right;
    int32_t              up_down;
} qcff_gaze_deg_t;

/* Face direction in degree in three axes */
typedef struct {
    int32_t              up_down_in_degree;
    int32_t              left_right_in_degree;
    int32_t              roll_in_degree;
} qcff_face_dir_t;

typedef struct {
    int32_t              confidence;
    qcff_rect_t          rect;
    int32_t              roll;
    int32_t              yaw;
} qcff_eye_t;

/* Face feature */
typedef void* qcff_face_feature_t;

/* Complete information about the detected faces.
   This is used in conjunction to qcff_get_complete_info.
   Each field in the structure points to an array
   for holding various information about the faces.
   When the pointer to the array is empty,
   qcff_get_complete_info will skip fetching the
   information. The length (number of entries in each
   array needs to be the same). Example:

   |====================|
   | complete face info |
   |====================|    |-----------------------------------------------|
   | p_rects           -+--> |   rect1   |   rect2   |   rect3   |   rect4   |
   |--------------------|    |-----------------------------------------------|
   | p_parts           -+--> NULL
   |--------------------|
   | p_parts_ex        -+--> NULL
   |--------------------|
   | p_directions      -+--> NULL
   |--------------------|    |-----------------------------------------------|
   | p_smile_degrees   -+--> |   smile1  |   smile2  |   smile3  |   smile4  |
   |--------------------|    |-----------------------------------------------|
   | p_eye_open_degrees-+--> NULL
   |--------------------|
   | p_gaze_degrees    -+--> NULL
   |====================|

   When this is passed to qcff_get_complete_info, all 4 rectangles and
   smile degrees will be filled out by the function.

   */
typedef struct {
    qcff_face_rect_t      *p_rects;
    qcff_face_parts_t     *p_parts;
    qcff_face_parts_ex_t  *p_parts_ex;
    qcff_face_dir_t       *p_directions;
    uint32_t              *p_smile_degrees;
    qcff_eye_open_deg_t   *p_eye_open_degrees;
    qcff_gaze_deg_t       *p_gaze_degrees;
} qcff_complete_face_info_t;

/* Opaque handle to an QCFF instance */
typedef void* qcff_handle_t;

/*************************************************************************
 * qcff_create
 *
 * All QCFF interface functions (except get version) require the handle
 * to an instance of QCFF as the first parameter. It encapsulates all
 * necessary memory and session management inside. This function creates
 * such a handle for subsequent use. The handle should be destroyed by
 * qcff_destroy at the end of use.
 *
 * OUTPUT:       p_handle    Created handle if operation successful.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_NO_RESOURCE
 ************************************************************************/
int qcff_create (qcff_handle_t     *p_handle);

/*************************************************************************
 * qcff_config
 *
 * This function provides mandatory configurations to the QCFF instance.
 * It includes the input image resolution (width and height). QCFF will
 * read all subsequent inputs assuming the same resolution. It currently
 * assumes and accepts only 8-bit grayscale images.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 *               p_cfg      Pointer to the configuration structure.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_config (qcff_handle_t      handle,
                 qcff_config_t     *p_cfg);

/*************************************************************************
 * qcff_set_frame
 *
 * This function provides the input to the QCFF instance. A local copy of
 * the frame will be made and therefore the input frame can be released
 * and altered freely after this call is finished.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_set_frame (qcff_handle_t   handle,
                    uint8_t        *p_frame);

/*************************************************************************
 * qcff_set_mode
 *
 * This function sets the optional parameter: detection mode.
 * It should be configured differently when QCFF is used in still image
 * vs video sequence scenario. If unset, default is QCFF_MODE_VIDEO.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_set_mode (qcff_handle_t     handle,
                   qcff_mode_t       mode);

int qcff_set_engine (qcff_handle_t   handle,
                     qcff_engine_t   engine);

/*************************************************************************
 * qcff_get_num_faces
 *
 * This function retrieves the number of detected faces in the frame
 * received in the last 'qcff_set_frame' call.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_get_num_faces (qcff_handle_t  handle,
                        uint32_t      *p_num_faces);

/*************************************************************************
 * qcff_get_complete_info
 *
 * This function retrieves the all possible information about each face
 * such as locations, directions, smile degrees, eye open-close degrees, etc.
 * It allows multiple faces to be retrieved by taking an array of face
 * indices corresponding to the information requested. The output
 * is a number of arrays holding the information requested allocated by the
 * caller of the function. Example:
 *
 * |====================|
 * | complete face info |
 * |====================|    |-----------------------------------------------|
 * | p_rects           -+--> |   rect1   |   rect2   |   rect3   |   rect4   |
 * |--------------------|    |-----------------------------------------------|
 * | p_parts           -+--> NULL
 * |--------------------|
 * | p_parts_ex        -+--> NULL
 * |--------------------|
 * | p_directions      -+--> NULL
 * |--------------------|    |-----------------------------------------------|
 * | p_smile_degrees   -+--> |   smile1  |   smile2  |   smile3  |   smile4  |
 * |--------------------|    |-----------------------------------------------|
 * | p_eye_open_degrees-+--> NULL
 * |--------------------|
 * | p_gaze_degrees    -+--> NULL
 * |====================|
 *
 * When this is passed to qcff_get_complete_info, all 4 rectangles and
 * smile degrees will be filled out by the function.
 * The face indices are zero-based and should be between 0 to the number of
 * detected faces - 1.
 *
 * INPUT:        handle               Handle to QCFF instance created
 *                                    previously.
 *               num_faces_queried    The number of faces being queried.
 *                                    (it determines how many entries in
 *                                    p_face_indices to be read)
 *               p_face_indices       The indices of the faces in which
 *                                    the complete information is
 *                                    requested. Whenever an invalid
 *                                    face index is encountered, the
 *                                    function call ends.
 *                                    p_num_faces_returned indicates how
 *                                    many faces are returned before
 *                                    the error happened.
 * OUTPUT:       p_num_faces_returned Returns the actual number of
 *                                    entries returned. It might be
 *                                    less than the number of faces queried
 *                                    if the number queried is actually
 *                                    more than the available faces.
 *               p_complete_info      The complete info outputted.

 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_get_complete_info (qcff_handle_t               handle,
                            uint32_t                    num_faces_queried,
                            uint32_t                   *p_face_indices,
                            uint32_t                   *p_num_faces_returned,
                            qcff_complete_face_info_t  *p_complete_info);


int qcff_create_feature_cache  (qcff_handle_t           handle,
                                uint32_t                face_index,
                                qcff_face_feature_t*    p_feature);
//int qcff_destroy_feature_cache (qcff_face_feature_t     feature);

/*************************************************************************
 * qcff_reg_new_usr
 *
 * This function registers a new user in the internal database. It
 * requires the association of one of the detected faces. The face index
 * is zero-based and should be between 0 to the number of detected faces-1.
 * When the operation is successful, the new user ID is returned. This is
 * used to subsequently refer to this user during identification.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 *               feature    Feature obtained earlier through
 *                          qcff_create_feature_cache.
 * OUTPUT:       p_new_user_id  The user ID of the new user returned.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_reg_new_usr (qcff_handle_t        handle,
                      qcff_face_feature_t  feature,
                      uint32_t            *p_new_user_id);

/*************************************************************************
 * qcff_reg_ex_usr
 *
 * This function registers an existing user. This improves the face
 * recognition accuracy by learning faces of the same person under
 * different lighting conditions, poses and out-of-plane rotations.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 *               feature    Feature obtained earlier through
 *                          qcff_create_feature_cache.
 *               user_id    The user ID of the existing user.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_reg_ex_usr (qcff_handle_t        handle,
                     qcff_face_feature_t  feature,
                     uint32_t             user_id);

/*************************************************************************
 * qcff_remove_ex_usr
 *
 * This function removes an existing user from the database.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 *               user_id    The user ID of the existing user to be removed.
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_remove_ex_usr (qcff_handle_t        handle,
                        uint32_t             user_id);

/*************************************************************************
 * qcff_identify_usr
 *
 * This function performs face recognition by matching the subject face
 * to the bank of previously registered faces. The result is the ID of
 * the user which has the closest match to the subject face. It also
 * outputs the confidence level of the claimed match. Caller can react
 * differently depending on the confidence level, such as asking the
 * user to confirm its identity if the confidence level is not high.
 * This gives the opportunity for this new face to be registered also
 * to improve accuracy later.
 *
 * INPUT:        handle       Handle to QCFF instance created previously.
 *               face_index   Zero-based index to indicate which detected
 *                            face is to be used for matching.
 * OUTPUT:       p_user_id    The ID of the user having the closest match
 *                            against the subject face. This is only
 *                            meaningful when the return value of the
 *                            function call is QCFF_RET_SUCCESSFUL.
 *                            In any other case, this output argument
 *                            is left untouched.
 *               p_confidence The confidence level of the match. Same with
 *                            p_user_id, it is valid only when return value
 *                            of this call is QCFF_RET_SUCCESSFUL.
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 *               QCFF_RET_NO_MATCH
 ************************************************************************/
int qcff_identify_usr (qcff_handle_t        handle,
                       uint32_t             face_index,
                       uint32_t            *p_user_id,
                       //qcff_confidence_t   *p_confidence);
                       uint32_t            *p_confidence);

/*************************************************************************
 * qcff_get_num_ex_usrs
 *
 * This function queries for the total number of registered users.
 *
 * INPUT:        handle       Handle to QCFF instance created previously.
 * OUTPUT:       p_num_users  The number of previously registered users.
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_get_num_ex_usrs (qcff_handle_t  handle,
                          uint32_t      *p_num_users);

/*************************************************************************
 * qcff_reset_usr_data
 *
 * This functions resets all registration/user data.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_reset_usr_data (qcff_handle_t  handle);

/*************************************************************************
 * qcff_get_usr_data_size
 *
 * For recognition to be useful, typically the user data needs to be
 * remembered across sessions. QCFF provides a set of functions to
 * allow easy storing and restoring of its user data bank. To save the
 * user data, QCFF provides the qcff_get_usr_data function to serialize
 * the user data and writes it to a buffer. The caller can then saves
 * what is in the buffer to persistent storage such as the file system.
 * This function (qcff_get_usr_data_size) queries the size of the user
 * data when serialized. This should be called to prepare for the memory
 * buffer for the saving of user data.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 * OUTPUT:       p_size     The size needed to hold the serialized the
 *                          user data.
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_get_usr_data_size (qcff_handle_t   handle,
                            uint32_t       *p_size);

/*************************************************************************
 * qcff_get_usr_data
 *
 * For recognition to be useful, typically the user data needs to be
 * remembered across sessions. QCFF provides a set of functions to
 * allow easy storing and restoring of its user data bank. To save the
 * user data, QCFF provides the qcff_get_usr_data function to serialize
 * the user data and writes it to a buffer. The caller can then saves
 * what is in the buffer to persistent storage such as the file system.
 * Caller should use qcff_get_usr_data_size in advance to query the size
 * of the user data when serialized in order to allocate for the memory
 * buffer big enough to hold the serialized user data.
 *
 * INPUT:        handle               Handle to QCFF instance created
 *                                    previously.
 *               p_buffer             The buffer to be used for holding
 *                                    the serialized user data.
 *               num_bytes_in_buffer  The total number of bytes
 *                                    available in the buffer.
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_get_usr_data (qcff_handle_t   handle,
                       uint8_t        *p_buffer,
                       uint32_t        num_bytes_in_buffer);

/*************************************************************************
 * qcff_set_usr_data
 *
 * This functions takes previously serialized user data (through
 * qcff_get_usr_data) and restore the user data bank from it.
 *
 * INPUT:        handle              Handle to QCFF instance created
 *                                   previously.
 *               num_bytes_in_buffer The number of bytes in the buffer.
 *               p_buffer            The buffer holding the serialized
 *                                   user data.
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_set_usr_data (qcff_handle_t  handle,
                       uint32_t       num_bytes_in_buffer,
                       uint8_t       *p_buffer);

/*************************************************************************
 * qcff_destroy
 *
 * This function destroys and performs clean up associated with the
 * QCFF instance previously created.
 *
 * INPUT:        handle     Handle to QCFF instance created previously.
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_destroy (qcff_handle_t *p_handle);

/*************************************************************************
 * qcff_setThreshold
 *
 * This function sets the confidence threshold value.
 *
 * INPUT:        threshold     threshold value.
 * 				 
 *
 * RETURN VALUE: QCFF_RET_SUCCESS
 *               QCFF_RET_INVALID_PARM
 ************************************************************************/
int qcff_setThreshold (int32_t  threshold);

#endif
