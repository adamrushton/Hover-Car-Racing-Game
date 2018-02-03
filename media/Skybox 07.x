xof 0303txt 0032
template Frame {
 <3d82ab46-62da-11cf-ab39-0020af71e433>
 [...]
}

template Matrix4x4 {
 <f6f23f45-7686-11cf-8f52-0040333594a3>
 array FLOAT matrix[16];
}

template FrameTransformMatrix {
 <f6f23f41-7686-11cf-8f52-0040333594a3>
 Matrix4x4 frameMatrix;
}

template Vector {
 <3d82ab5e-62da-11cf-ab39-0020af71e433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template MeshFace {
 <3d82ab5f-62da-11cf-ab39-0020af71e433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template Mesh {
 <3d82ab44-62da-11cf-ab39-0020af71e433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template MeshNormals {
 <f6f23f43-7686-11cf-8f52-0040333594a3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template Coords2d {
 <f6f23f44-7686-11cf-8f52-0040333594a3>
 FLOAT u;
 FLOAT v;
}

template MeshTextureCoords {
 <f6f23f40-7686-11cf-8f52-0040333594a3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template ColorRGBA {
 <35ff44e0-6c7c-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <d3e16e81-7835-11cf-8f52-0040333594a3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template Material {
 <3d82ab4d-62da-11cf-ab39-0020af71e433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshMaterialList {
 <f6f23f42-7686-11cf-8f52-0040333594a3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material <3d82ab4d-62da-11cf-ab39-0020af71e433>]
}

template TextureFilename {
 <a42790e1-7810-11cf-8f52-0040333594a3>
 STRING filename;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}


Frame {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Mesh single_mesh_object {
	20;
	-1000.000000;0.000000;1000.000000;,
	-1000.000000;0.000000;1000.000000;,
	-1000.000000;0.000000;-1000.000000;,
	-1000.000000;0.000000;-1000.000000;,
	-1000.000000;2000.000000;1000.000000;,
	-1000.000000;2000.000000;1000.000000;,
	-1000.000000;2000.000000;-1000.000000;,
	-1000.000000;2000.000000;-1000.000000;,
	-1000.000000;2000.000000;-1000.000000;,
	1000.000000;0.000000;1000.000000;,
	1000.000000;0.000000;1000.000000;,
	1000.000000;0.000000;1000.000000;,
	1000.000000;0.000000;-1000.000000;,
	1000.000000;0.000000;-1000.000000;,
	1000.000000;0.000000;-1000.000000;,
	1000.000000;2000.000000;1000.000000;,
	1000.000000;2000.000000;1000.000000;,
	1000.000000;2000.000000;-1000.000000;,
	1000.000000;2000.000000;-1000.000000;,
	1000.000000;2000.000000;-1000.000000;;

	12;
	3;16,19,14;,
	3;11,16,14;,
	3;10,1,5;,
	3;15,10,5;,
	3;8,4,0;,
	3;3,8,0;,
	3;18,7,2;,
	3;13,18,2;,
	3;16,4,6;,
	3;17,16,6;,
	3;12,2,1;,
	3;9,12,1;;

  MeshNormals {
		8;
		-0.577350;-0.577350;0.577350;,
		-0.577350;-0.577350;-0.577350;,
		-0.577350;0.577350;0.577350;,
		-0.577350;0.577350;-0.577350;,
		0.577350;-0.577350;0.577350;,
		0.577350;-0.577350;-0.577350;,
		0.577350;0.577350;0.577350;,
		0.577350;0.577350;-0.577350;;

		12;
		3;1,0,2;,
		3;3,1,2;,
		3;3,7,5;,
		3;1,3,5;,
		3;4,5,7;,
		3;6,4,7;,
		3;0,4,6;,
		3;2,0,6;,
		3;1,5,4;,
		3;0,1,4;,
		3;2,6,7;,
		3;3,2,7;;
  }

  MeshTextureCoords {
		20;
		0.001302;0.998698;,
		0.998698;0.998698;,
		0.001302;0.998698;,
		0.998698;0.998698;,
		0.001302;0.001302;,
		0.998698;0.001302;,
		0.001302;0.998698;,
		0.001302;0.001302;,
		0.998698;0.001302;,
		0.998698;0.001302;,
		0.001302;0.998698;,
		0.998698;0.998698;,
		0.001302;0.001302;,
		0.998698;0.998698;,
		0.001302;0.998698;,
		0.001302;0.001302;,
		0.998698;0.001302;,
		0.998698;0.998698;,
		0.998698;0.001302;,
		0.001302;0.001302;;
  }

  MeshMaterialList {
		6;
		12;
		0,0,1,1,2,2,3,3,4,4,5,5;

		Material {
			1.0;1.0;1.0;1.000000;;
			0.000000;
			0.220000;0.220000;0.220000;;
			0.000000;0.000000;0.000000;;
			TextureFilename {
				"front.jpg";
			}
		}

		Material {
			1.0;1.0;1.0;1.000000;;
			0.000000;
			0.220000;0.220000;0.220000;;
			0.000000;0.000000;0.000000;;
			TextureFilename {
				"right.jpg";
			}
		}

		Material {
			1.0;1.0;1.0;1.000000;;
			0.000000;
			0.220000;0.220000;0.220000;;
			0.000000;0.000000;0.000000;;
			TextureFilename {
				"back.jpg";
			}
		}

		Material {
			1.0;1.0;1.0;1.000000;;
			0.000000;
			0.220000;0.220000;0.220000;;
			0.000000;0.000000;0.000000;;
			TextureFilename {
				"left.jpg";
			}
		}

		Material {
			1.0;1.0;1.0;1.000000;;
			0.000000;
			0.220000;0.220000;0.220000;;
			0.000000;0.000000;0.000000;;
			TextureFilename {
				"top.jpg";
			}
		}

		Material {
			1.0;1.0;1.0;1.000000;;
			0.000000;
			0.220000;0.220000;0.220000;;
			0.000000;0.000000;0.000000;;
			TextureFilename {
				"bottom.jpg";
			}
		}
  }
 }
}