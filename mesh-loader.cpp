#include "mesh-loader.h"

/*
 *  mesh_loader.cpp
 *  ACCtessellator
 *
 *  Created by Denis Kovacs on 8/9/08.
 *  Copyright 2008 NYU. All rights reserved.
 *
 */



#define NO_VTX (unsigned int)(-1)



//------------------------------------------------------------------
namespace MeshLoad {

  struct VTXindex_ltstr
  {
    bool operator()(VTXindex v1, VTXindex v2) const
    {
      if (v1.posIdx != v2.posIdx) return v1.posIdx<v2.posIdx;
      if (v1.uvIdx  != v2.uvIdx) return v1.uvIdx<v2.uvIdx;
      return v1.norIdx<v2.norIdx;
    }
  };

  typedef std::map<VTXindex, int, VTXindex_ltstr> VTXmap;

  //------------------------------------------------------------------
  static void skipLine(std::istream& ifs);
  static bool skipCommentLine(std::istream& ifs);


  // ripped from TooL

  struct OBJMesh* readOBJ(const char *filename)
  //------------------------------------------------------------------
  {
    OBJMesh*  mesh = new OBJMesh;
	
    std::string ele_id;
    float x, y, z;
	
    std::ifstream ifs( filename );
    if (!ifs) {
      printf("failed to open %s\n", filename);
      return mesh;
    }
    while(skipCommentLine(ifs)) 
      {
	if (!(ifs >> ele_id)) return mesh;
		
	if ("v" == ele_id)            // position
	  {
	    ifs >> x >> y >> z;
	    mesh->pos.push_back(Vec3(x, y, z));
	  }
	else if ("vt" == ele_id) 	  // uv coordinates
	  {
	    ifs >> x >> y >> z;
	    ifs.clear();                // is z (i.e. w) is not available, have to clear error flag.
	    mesh->uv.push_back(Vec2(x, y));
	  }
	else if ("vn" == ele_id)      // normal
	  {	
	    ifs >> x >> y >> z;
			
	    if(!ifs.good()) 
	      {                         // in case it is -1#IND00
		x = y = z = 0.0;
		ifs.clear();
		skipLine(ifs);
	      }
	    mesh->nor.push_back(Vec3(x, y, z));
	  }
	else if ("f" == ele_id) //	face data
	  {	
	    // record face size
	    mesh->face_startidx.push_back(mesh->faces.size());
	    // read in all vertices of a face
	    std::vector<VTXindex> indices;
	    while ( ifs.good() )
	      {
		char c;
		VTXindex v = VTXindex(NO_VTX, NO_VTX, NO_VTX);
		if(!mesh->hasTextureCoords() && !mesh->hasNormals()) {
		  ifs >> v.posIdx;
		  v.posIdx--;
		} else if(!mesh->hasTextureCoords()) {
		  ifs >> v.posIdx >> c >> c >> v.norIdx;
		  v.posIdx--;  v.norIdx--;
		} else if(!mesh->hasNormals()) {
		  ifs >> v.posIdx >> c >> v.uvIdx;
		  v.posIdx--;  v.uvIdx--;
		} else {
		  ifs >> v.posIdx >> c >> v.uvIdx >> c >>  v.norIdx;
		  v.posIdx--;  v.uvIdx--;  v.norIdx--;
		}

		if ( ifs.good() ) indices.push_back(v);
	      }

	    for (int k=0; k< indices.size(); k++)
	      {
		mesh->faces.push_back(indices[k]);				
	      }	

	    ifs.clear();
	  }
	else
	  skipLine(ifs);
		
      }	
		
    ifs.close();
	
    return mesh;
  }




  static void skipLine(std::istream& ifs)
  {
    char next;
    ifs >> std::noskipws;
    while( (ifs >> next) && ('\n' != next) );
  }

  static bool skipCommentLine(std::istream& ifs)
  {
    char next;
    while( ifs >> std::skipws >> next ) 
      {
	ifs.putback(next);
	if ('#' == next)
	  skipLine(ifs);
	else
	  return true;
      }
    return false;
  }


  void dumpMeshVerbose(const OBJMesh& mesh)
  //------------------------------------------------------------------
  {
    int nV;
    unsigned int i,j;

    printf("#nV:%d nF:%d",
	   (int)mesh.pos.size(), (int)mesh.face_startidx.size());
	
    printf(" Pos:\n");
    nV = 0;
    for(i=0; i<mesh.pos.size(); i++ )
      {
	printf("#%4d:  ",i);
	printf("v %f %f %f\n", mesh.pos[nV].x(),mesh.pos[nV].y(),mesh.pos[nV].z() ); nV++;
      }
	
    if (mesh.uv.size() > 0 )
      {
	printf("# UV:  ");
	nV = 0;
	for(i=0; i<mesh.uv.size(); i++ )
	  {
	    printf("#%4d: \n",i);
	    printf("vt %f %f\n", mesh.uv[nV].x(),mesh.uv[nV].y()); nV++;
	  }
      }
	
    if (mesh.nor.size() > 0 )
      {
	printf("# Nor:  ");
	nV = 0;
	for(i=0; i<mesh.nor.size(); i++)
	  {			printf("#%4d: ",i);
	    printf("vn %f %f %f\n", mesh.nor[nV].x(),mesh.nor[nV].y(),mesh.nor[nV].z() ); nV++;
	  }
      }
	
    if (mesh.faces.size() )
      {
	printf("# Faces:\n");
	nV = 0;
	for(i=0; i<mesh.face_startidx.size(); i++)
	  {
	    printf("# %4d: f ",i);
	    unsigned int endidx = 0; 
	    if (i < mesh.face_startidx.size()-1) endidx = mesh.face_startidx[i+1]; else endidx = mesh.faces.size();
	    for(j=mesh.face_startidx[i]; j< endidx; j++ )
	      {
		printf("%4d ", mesh.faces[nV].posIdx); 
		if( mesh.faces[nV].uvIdx != NO_VTX) printf("/%4d ", mesh.faces[nV].uvIdx); 
		if( mesh.faces[nV].norIdx != NO_VTX) printf("/%4d ", mesh.faces[nV].norIdx); 
		nV++;
	      }
	    printf("\n");
	  }
      }
  }
};
