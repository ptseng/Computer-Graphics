//
//  Statute.cpp
//  AmusementPark
//
//  Created by Philip Tseng on 11/17/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#include <iostream>
#include "Statute.h"
#include <Fl/gl.h>

Statute::Statute()
{
    num_vertices = 6;
    num_edges = 12;
    num_faces = 4;
    
    vertices = new Vertex[num_vertices];
    edges = new Edge[num_edges];
    faces = new Triangle[num_faces];
    
    // A
    vertices[0].x[0] = -1.0f;
    vertices[0].x[1] = 1.0f;
    vertices[0].x[2] = 1.0f;
    
    // B
    vertices[1].x[0] = 1.0f;
    vertices[1].x[1] = 1.0f;
    vertices[1].x[2] = 0.0f;
    
    // C
    vertices[2].x[0] = -1.0f;
    vertices[2].x[1] = -1.0f;
    vertices[2].x[2] = 1.0f;
    
    // D
    vertices[3].x[0] = 1.0f;
    vertices[3].x[1] = -1.0f;
    vertices[3].x[2] = 0.0f;
    
    // E
    vertices[4].x[0] = -1.0f;
    vertices[4].x[1] = -1.0f;
    vertices[4].x[2] = 0.0f;
    
    // F
    vertices[5].x[0] = -1.0f;
    vertices[5].x[1] = 1.0f;
    vertices[5].x[2] = 0.0f;
    
    //CA
    edges[0].vs = 2; edges[0].ve = 0;
    
    //AB
    edges[1].vs = 0; edges[1].ve = 1;
    
    //BC
    edges[2].vs = 1; edges[2].ve = 2;
    
    //CD
    edges[3].vs = 2; edges[3].ve = 3;
    
    //DB
    edges[4].vs = 3; edges[4].ve = 1;
    
    //FA
    edges[5].vs = 4; edges[5].ve = 0;
    
    //EC
    edges[6].vs = 4; edges[6].ve = 2;
    
    //DE
    edges[7].vs = 3; edges[7].ve = 4;
    
    //BF
    edges[8].vs = 1; edges[8].ve = 5;
    
    //EF
    edges[9].vs = 4; edges[9].ve = 5;
    
    //DF
    edges[10].vs = 3; edges[10].ve = 5;
    
    //CF
    edges[11].vs = 2; edges[11].ve = 5;
    
    //FACE CAB
    faces[0].edges[0] = 0; faces[0].forward[0] = true;
    faces[0].edges[1] = 1; faces[0].forward[1] = true;
    faces[0].edges[2] = 2; faces[0].forward[2] = true;
    
    //FACE CDB
    faces[1].edges[0] = 3; faces[1].forward[0] = true;
    faces[1].edges[1] = 4; faces[1].forward[1] = true;
    faces[1].edges[2] = 2; faces[1].forward[2] = true;
    
    //FACE CDE
    faces[2].edges[0] = 3; faces[2].forward[0] = true;
    faces[2].edges[1] = 7; faces[2].forward[1] = true;
    faces[2].edges[2] = 6; faces[2].forward[2] = true;
    
    //FACE ABF
    faces[3].edges[0] = 1; faces[3].forward[0] = true;
    faces[3].edges[1] = 8; faces[3].forward[1] = true;
    faces[3].edges[2] = 5; faces[3].forward[2] = true;
}


Statute::~Statute()
{
    delete[] vertices;
    delete[] edges;
    delete[] faces;
}


void Statute::Subdivide(int n)
{
    int    num_v;
    Vertex	    *new_v;
    int    num_e;
    Edge	    *new_e;
    int    num_f;
    Triangle	    *new_f;
    int    i, j;
    
    // Copy old vertices over
    new_v = new Vertex[num_vertices + num_edges];
    
    for ( i = 0 ; i < num_vertices ; i++ )
    {
        new_v[i].x[0] = vertices[i].x[0];
        new_v[i].x[1] = vertices[i].x[1];
        new_v[i].x[2] = vertices[i].x[2];
    }
    
    num_v = num_vertices;
    
    // Split all the edges.
    new_e = new Edge[num_edges * 2 + num_faces * 3];
    num_e = 0;
    
    for ( i = 0 ; i < num_edges ; i++ )
    {
        double  l = 0.0f;
        
        for ( j = 0 ; j < 3 ; j++ )
        {
            new_v[num_v].x[j] = ( vertices[edges[i].vs].x[j] + vertices[edges[i].ve].x[j] ) * 0.50f;
            l += new_v[num_v].x[j] * new_v[num_v].x[j];
        }
        
        l = sqrt(l);
        
        new_v[num_v].x[0] /= (double)l;
        new_v[num_v].x[1] /= (double)l;
        new_v[num_v].x[2] /= (double)l;
        
        edges[i].v_new = num_v;
        
        new_e[num_e].vs = edges[i].vs;
        new_e[num_e].ve = num_v;
        edges[i].s_child = num_e++;
        
        new_e[num_e].vs = num_v;
        new_e[num_e].ve = edges[i].ve;
        edges[i].e_child = num_e++;
        
        num_v++;
    }
    
    // Re-work the faces.
    new_f = new Triangle[num_faces * 4];
    
    num_f = 0;
    
    for ( i = 0 ; i < num_faces ; i++ )
    {
        int	e1 = num_e++;
        int	e2 = num_e++;
        int	e3 = num_e++;
        TrianglePtr 	f;
        
        new_e[e1].vs = edges[faces[i].edges[0]].v_new;
        new_e[e1].ve = edges[faces[i].edges[1]].v_new;
        
        new_e[e2].vs = edges[faces[i].edges[1]].v_new;
        new_e[e2].ve = edges[faces[i].edges[2]].v_new;
        
        new_e[e3].vs = edges[faces[i].edges[2]].v_new;
        new_e[e3].ve = edges[faces[i].edges[0]].v_new;
        
        f = &(new_f[num_f++]);
        if ( faces[i].forward[0] )
        {
            f->edges[0] = edges[faces[i].edges[0]].e_child;
            f->forward[0] = true;
        }
        else
        {
            f->edges[0] = edges[faces[i].edges[0]].s_child;
            f->forward[0] = false;
        }
        if ( faces[i].forward[1] )
        {
            f->edges[1] = edges[faces[i].edges[1]].s_child;
            f->forward[1] = true;
        }
        else
        {
            f->edges[1] = edges[faces[i].edges[1]].e_child;
            f->forward[1] = false;
        }
        f->edges[2] = e1;
        f->forward[2] = false;
        
        f = &(new_f[num_f++]);
        f->edges[0] = e1;
        f->forward[0] = true;
        f->edges[1] = e2;
        f->forward[1] = true;
        f->edges[2] = e3;
        f->forward[2] = true;
        
        f = &(new_f[num_f++]);
        f->edges[0] = e2;
        f->forward[0] = false;
        if ( faces[i].forward[1] )
        {
            f->edges[1] = edges[faces[i].edges[1]].e_child;
            f->forward[1] = true;
        }
        else
        {
            f->edges[1] = edges[faces[i].edges[1]].s_child;
            f->forward[1] = false;
        }
        if ( faces[i].forward[2] )
        {
            f->edges[2] = edges[faces[i].edges[2]].s_child;
            f->forward[2] = true;
        }
        else
        {
            f->edges[2] = edges[faces[i].edges[2]].e_child;
            f->forward[2] = false;
        }
        
        f = &(new_f[num_f++]);
        f->edges[0] = e3;
        f->forward[0] = false;
        if ( faces[i].forward[2] )
        {
            f->edges[1] = edges[faces[i].edges[2]].e_child;
            f->forward[1] = true;
        }
        else
        {
            f->edges[1] = edges[faces[i].edges[2]].s_child;
            f->forward[1] = false;
        }
        if ( faces[i].forward[0] )
        {
            f->edges[2] = edges[faces[i].edges[0]].s_child;
            f->forward[2] = true;
        }
        else
        {
            f->edges[2] = edges[faces[i].edges[0]].e_child;
            f->forward[2] = false;
        }
    }
    
    delete[] vertices;
    delete[] edges;
    delete[] faces;
    
    num_vertices = num_v;
    vertices = new_v;
    num_edges = num_e;
    edges = new_e;
    num_faces = num_f;
    faces = new_f;
    
    if ( n > 1 )
        Subdivide(n - 1);
}


void Statute::Render(bool smooth)
{
    float l;
    int j;
    
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    
    //glColor3f(255.0f, 255.0f, 255.0f);
    glTranslatef(0, 0, 10);
    //glRotated(90, 0, 0, 1);
    glScalef(20, 20, 20);
    
    
    
    for ( int i = 0 ; i < num_faces ; ++i )
    {
        glBegin(GL_LINE_STRIP);
        
        int    a, b, c;
        float		v1[3];
        float		v2[3];
        float   	n[3];
        
        a = faces[i].forward[0] ? edges[faces[i].edges[0]].vs : edges[faces[i].edges[0]].ve;
        b = faces[i].forward[1] ? edges[faces[i].edges[1]].vs : edges[faces[i].edges[1]].ve;
        c = faces[i].forward[2] ? edges[faces[i].edges[2]].vs : edges[faces[i].edges[2]].ve;
        
        std::cout << "V1: (" << vertices[a].x[0] << "," << vertices[a].x[1] << "," << vertices[a].x[2] << ")" << std::endl;
        std::cout << "V2: (" << vertices[b].x[0] << "," << vertices[b].x[1] << "," << vertices[b].x[2] << ")" << std::endl;
        std::cout << "V3: (" << vertices[c].x[0] << "," << vertices[c].x[1] << "," << vertices[c].x[2] << ")" << std::endl;
        
        if ( smooth )
        {
            glNormal3fv(vertices[a].x);
            glVertex3fv(vertices[a].x);
            
            glNormal3fv(vertices[b].x);
            glVertex3fv(vertices[b].x);
            
            glNormal3fv(vertices[c].x);
            glVertex3fv(vertices[c].x);
            
        }
        else
        {
            for ( j = 0 ; j < 3 ; j++ )
            {
                v1[j] = vertices[b].x[j] - vertices[a].x[j];
                v2[j] = vertices[c].x[j] - vertices[a].x[j];
            }
            n[0] = v1[1] * v2[2] - v1[2] * v2[1];
            n[1] = v1[2] * v2[0] - v1[0] * v2[2];
            n[2] = v1[0] * v2[1] - v1[1] * v2[0];
            l = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
            n[0] /= (float)l;
            n[1] /= (float)l;
            n[2] /= (float)l;
            
            glNormal3fv(n);
            glVertex3fv(vertices[a].x);
            glVertex3fv(vertices[b].x);
            glVertex3fv(vertices[c].x);
        }
        glEnd();
    }

}


