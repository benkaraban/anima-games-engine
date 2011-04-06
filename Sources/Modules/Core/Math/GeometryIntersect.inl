#include <Core/Math/MathTools.h>
#include <Core/Math/Operation.h>

namespace Core
{
//------------------------------------------------------------------------------
GEOFUNCINLINE bool intersect_RayAABox(const Rayf& ray, const AABoxf& box, float &distanceMin)
{
    distanceMin = 0.f;
    float distanceMax = FLT_MAX;


    /////TODO TEST
    Core::Vector3f d = box.max-box.min;
    if ( d.getLengthSafe() < f_INTERSECT_EPSILON ) return false;
    /////FIN TEST

    //pour chaque axe
    for (int ii=0; ii<3; ii++)
    {
        if (L_abs(ray.direction[ii]) < f_INTERSECT_EPSILON)
        {
            // Le rayon est parallele a l'axe, pas d'intersection si la projection 
            // de l'origin sur le plan n'est pas  situe sur la face
            if (ray.origin[ii] < box.min[ii] || ray.origin[ii] > box.max[ii]) return false;
        }
        else
        {
            // on cherche la distance d'intersection avec les 2 plans du cube pour cette axe
            float ood = 1.f / ray.direction[ii];
            float t1 = (box.min[ii] - ray.origin[ii]) * ood;
            float t2 = (box.max[ii] - ray.origin[ii]) * ood;

            // t1 est la distance d'intersection avec le plan le plus proche de l'origine du rayon, t2 avec le plan le plus eloigne
            if (t1 > t2) Core::L_swap(t1, t2);
            
            if (t1 > distanceMin) distanceMin = t1;
            if (t2 < distanceMax) distanceMax = t2;
            
            // si l'intersection est vide
            if (distanceMin > distanceMax) 
                return false;
        }
    }

    return true;
}
//------------------------------------------------------------------------------
GEOFUNCINLINE bool intersect_RayTriangle(const Rayf& ray, const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, Vector3f& position)
{
    //les vecteur decrivant les deux arrètes issue de p0
    Vector3f edge1 = p1-p0;
    Vector3f edge2 = p2-p0;
    
    //debut du calcul du determinant
    Vector3f pvec  =  cross(ray.direction, edge1); 
    float det = dot(edge2, pvec);
    
    //si le determinant est proche de 0, le rayon se trouve dans le plan du triangle
    if (det<=0) return false;
    
    //la distance de l'origin du rayon au point p0
    Vector3f tvec = ray.origin - p0;
    
    //calcul du parametre U et test
    float u = dot(tvec, pvec);
    if (u<0.f || u>det) return false;
    
    //calcul du parametre V et test
    Vector3f qvec = cross(tvec, edge2);
    float v = dot(ray.direction, qvec);
    if ( v<0.0f || (u+v)>det) return false;
    
    //------------------------------------------------------
    // on a une intersection : calcul de la position 
    //------------------------------------------------------
    float t = dot(edge1, qvec) / det; // t = la distance de l'intersection à partir de l'origin du rayon
    position = (ray.direction*t) + ray.origin;
    
    //on ne prend pas en compte l'intersection si elle est dans la direction opposee
    if (dot((position-ray.origin), ray.direction) < 0.f) return false;
    
    return true;
}
//------------------------------------------------------------------------------
GEOFUNCINLINE bool intersect_RayPlane(const Rayf &ray, const Planef& plane, Vector3f& position)
{
    //TODO code temporaire


    position = plane.project(ray.origin, ray.direction) ;
    return true;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
 * l'AABox a quatre diagonales qui passent par son centre. On cherche la diagonale de la boite qui est la plus 
 * aligné avec la normal du plan. On prend alors les deux sommets de cette diagonal et on verifie de quel 
 * coté du plan il sont situés. 
 *   - L'AABox interst le plan si les signes diffère (ou si l'un est a zero) alors la boite intersect le plan.
 *   - L'AABox est a l'exterieur du plan si elle est situé dans la moitie de l'espace (positif) définit par : normal-pt + distance > 0
 *   - L'AABox est a l'interieur du plan si elle est situé dans la moitie de l'espace (negatif) définit par : normal-pt + distance < 0
**/
GEOFUNCINLINE EIntersecResultType intersect_AABoxPlane_Accurate(const AABoxf& box, const Planef& plane)
{
    Vector3f boxDiaMin, boxDiaMax; //les sommets de la diagonal la plus alignee avec la normal du plan
    
    if (plane.normal.x>=0)  {boxDiaMin.x=box.min.x; boxDiaMax.x=box.max.x;}
    else                    {boxDiaMin.x=box.max.x; boxDiaMax.x=box.min.x;}
    
    if (plane.normal.y>=0)  {boxDiaMin.y=box.min.y; boxDiaMax.y=box.max.y;}
    else                    {boxDiaMin.y=box.max.y; boxDiaMax.y=box.min.y;}
    
    if (plane.normal.z>=0)  {boxDiaMin.z=box.min.z; boxDiaMax.z=box.max.z;}
    else                    {boxDiaMin.z=box.max.z; boxDiaMax.z=box.min.z;}
    
    if ((dot(plane.normal, boxDiaMin) - plane.distance) > 0) return IR_OUTSIDE;
    if ((dot(plane.normal, boxDiaMax) - plane.distance) < 0) return IR_INSIDE;
    
    return IR_OVERLAP;
}
//------------------------------------------------------------------------------
GEOFUNCINLINE bool intersect_AABoxPlane_Fast(const AABoxf& box, const Planef& plane)
{   
    float dotValue = -plane.distance;
    
    if (plane.normal.x>=0)    dotValue+=plane.normal.x*box.min.x;
    else                      dotValue+=plane.normal.x*box.max.x;
    
    if (plane.normal.y>=0)    dotValue+=plane.normal.y*box.min.y;
    else                      dotValue+=plane.normal.y*box.max.y;
    
    if (plane.normal.z>=0)    dotValue+=plane.normal.z*box.min.z; 
    else                      dotValue+=plane.normal.z*box.max.z;
    
    if (dotValue>0) return false;
    
    return true;
}
//------------------------------------------------------------------------------
GEOFUNCINLINE bool intersect_AABoxFrustum_Fast(const AABoxf& box, const Frustumf& f)
{
    /*float dotValue;

    for (int32 iPlane=0; iPlane<EFrustumPlan_COUNT; iPlane++)
    {
        dotValue = -f.planes[iPlane].distance;
        
        if (f.planes[iPlane].normal.x>=0) dotValue+=f.planes[iPlane].normal.x*box.min.x;
        else                              dotValue+=f.planes[iPlane].normal.x*box.max.x;
        
        if (f.planes[iPlane].normal.y>=0) dotValue+=f.planes[iPlane].normal.y*box.min.y;
        else                              dotValue+=f.planes[iPlane].normal.y*box.max.y;
        
        if (f.planes[iPlane].normal.z>=0) dotValue+=f.planes[iPlane].normal.z*box.min.z; 
        else                              dotValue+=f.planes[iPlane].normal.z*box.max.z;
        
        if (dotValue>0) return false;
    }
    
    return true;*/
    Vector3f boxDiaMin, boxDiaMax; 
    
    for (int32 iPlane=0; iPlane<EFrustumPlan_COUNT; iPlane++)
    {
        if (f.planes[iPlane].normal.x>=0)  {boxDiaMin.x=box.min.x; boxDiaMax.x=box.max.x;}
        else                               {boxDiaMin.x=box.max.x; boxDiaMax.x=box.min.x;}
        
        if (f.planes[iPlane].normal.y>=0)  {boxDiaMin.y=box.min.y; boxDiaMax.y=box.max.y;}
        else                               {boxDiaMin.y=box.max.y; boxDiaMax.y=box.min.y;}
        
        if (f.planes[iPlane].normal.z>=0)  {boxDiaMin.z=box.min.z; boxDiaMax.z=box.max.z;}
        else                               {boxDiaMin.z=box.max.z; boxDiaMax.z=box.min.z;}
        
        if ((dot(f.planes[iPlane].normal, boxDiaMin) - f.planes[iPlane].distance) > 0) return false;
    }
    
    return true;
}
//------------------------------------------------------------------------------


}//namespace Core

