#ifndef MESH_LOADER_H
#define MESH_LOADER_H

/*
 *  mesh_loader.h
 *  ACCtessellator
 *
 *  Created by Denis Kovacs on 8/9/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <map>
#include <exception>
#include "cvec3t.h"
#include "cvec2t.h"

#include <stdio.h>


namespace MeshLoad {
  typedef CVec3T<float> Vec3;
  typedef CVec2T<float> Vec2;

  // triple of indices used per vertex of a face: 
  //the index of vertex position, the index of vertex texture coords (uv), 
  // and the index of normal
  struct VTXindex
  {
    int posIdx;
    int uvIdx;
    int norIdx;
        
    VTXindex( int p, int uv, int n): posIdx(p), uvIdx(uv), norIdx(n) {}
    VTXindex() {}
  };


  struct OBJMesh 
  {
    std::vector<Vec3> pos;
    std::vector<Vec2> uv;
    std::vector<Vec3> nor;
    // triple of indices for each face vertex
    std::vector<VTXindex>  faces;
    // indices in array faces  where faces start
    std::vector<unsigned int>  face_startidx; 	
    bool hasTextureCoords() { return uv.size() != 0; }
    bool hasNormals() { return nor.size() != 0; }
  };

  struct OBJMesh* readOBJ(const char *filename);

  void dumpMeshVerbose(const OBJMesh& mesh);

};

#endif
