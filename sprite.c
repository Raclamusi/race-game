#include<stdio.h>
#include<Windows.h>

#include "./include/sprite.h"
#include "./include/graphics.h"
#include "./include/vector.h"

#define OBJ_LINE_BUFFER_SIZE 128
#define OBJ_WORD_BUFFER_SIZE 16

Sprite initSprite(const char *id, Image image) {
  Sprite sprite = {};
  memset(&sprite, 0, sizeof(sprite));
	memcpy_s(sprite.id, sizeof(sprite.id), id, min(sizeof(sprite.id), strlen(id)));
	sprite.scale[0] = 1.0;
	sprite.scale[1] = 1.0;
  sprite.scale[2] = 1.0;
	sprite.texture = image;
  sprite.children = initVector();
  return sprite;
}

void discardSprite(Sprite sprite) {
  freeVector(&sprite.indices);
  freeVector(&sprite.vertices);
  freeVector(&sprite.uv);
  freeVector(&sprite.uvIndices);
  freeVector(&sprite.children);
}

void drawSprite(Sprite *sprite) {
  Sprite *child;

  if(sprite->behaviour != NULL) {
    if(!sprite->behaviour(sprite)) return;
  }
	pushTransformation();
  translateTransformation(sprite->position[0], sprite->position[1], sprite->position[2]);
  rotateTransformation(sprite->angle[0], sprite->angle[1], sprite->angle[2]);
	// pushTransformation();
  // translateTransformation(sprite->shadowOffset[0], sprite->shadowOffset[1], 0.0);
	// scaleTransformation(sprite->shadowScale, sprite->shadowScale, 1.0);
	// if(sprite->shadowOffset[0] != 0.0F || sprite->shadowOffset[1] != 0.0F) fillBuffer(sprite->texture, TRUE);
	// popTransformation();
  resetIteration(&sprite->children);
  while((child = previousData(&sprite->children))) drawSprite(child);
  scaleTransformation(sprite->scale[0], sprite->scale[1], sprite->scale[2]);
	fillPolygons(sprite->vertices, sprite->indices, sprite->texture, sprite->uv, sprite->uvIndices);
	popTransformation();
}

void genPolygonsPlane(unsigned int width, unsigned int height, Vector *indices, Vector *vertices, Vector *uv, Vector *uvIndices) {
  int i;
  float halfWidth = width / 2.0F;
  float halfHeight = height / 2.0F;
  *indices = initVector();
  *vertices = initVector();
  *uv = initVector();
  *uvIndices = initVector();
  static unsigned long generated_indices[] = { 0, 1, 2, 1, 3, 2 };
  Vertex generated_vertices[] = {
    { { -halfWidth, -halfHeight, 0.0F, 1.0F }, 0 },
    { { -halfWidth, halfHeight, 0.0F, 1.0F }, 0 },
    { { halfWidth, -halfHeight, 0.0F, 1.0F }, 0 },
    { { halfWidth, halfHeight, 0.0F, 1.0F }, 0 },
  };
  float generated_uv[][2] = {
    { 0.0F, 0.0F }, { 0.0F, 1.0F }, { 1.0F, 0.0F }, { 1.0F, 1.0F },
  };
  static unsigned long generated_uvIndices[] = { 0, 1, 2, 1, 3, 2 };
  for(i = 0;i < 6;i++) {
    unsigned long *index = malloc(sizeof(unsigned long));
    unsigned long *uvIndex = malloc(sizeof(unsigned long));
    *index = generated_indices[i];
    *uvIndex = generated_uvIndices[i];
    push(indices, index);
    push(uvIndices, uvIndex);
  }
  for(i = 0;i < 4;i++) {
    Vertex *vertex = malloc(sizeof(Vertex));
    float *coords = malloc(2 * sizeof(float));
    *vertex = generated_vertices[i];
    coords[0] = generated_uv[i][0];
    coords[1] = generated_uv[i][1];
    push(vertices, vertex);
    push(uv, coords);
  }
}

void genPolygonsBox(unsigned int width, unsigned int height, unsigned int depth, Vector *indices, Vector *vertices, Vector *uv, Vector *uvIndices) {
  int i;
  float halfWidth = width / 2.0F;
  float halfHeight = height / 2.0F;
  float halfDepth = depth / 2.0F;
  *indices = initVector();
  *vertices = initVector();
  *uv = initVector();
  *uvIndices = initVector();
  static unsigned long generated_indices[] = {
    0, 1, 2, 1, 3, 2,
    4, 5, 6, 5, 7, 6,
    8, 9, 10, 9, 11, 10,
    12, 13, 14, 13, 15, 14,
    16, 17, 18, 17, 19, 18,
    20, 21, 22, 21, 23, 22,
  };
  Vertex generated_vertices[] = {
    { { -halfWidth, -halfHeight, halfDepth, 1.0F }, 0 },
    { { -halfWidth, halfHeight, halfDepth, 1.0F }, 0 },
    { { halfWidth, -halfHeight, halfDepth, 1.0F }, 0 },
    { { halfWidth, halfHeight, halfDepth, 1.0F }, 0 },

    { { -halfWidth, -halfHeight, -halfDepth, 1.0F }, 0 },
    { { halfWidth, -halfHeight, -halfDepth, 1.0F }, 0 },
    { { -halfWidth, halfHeight, -halfDepth, 1.0F }, 0 },
    { { halfWidth, halfHeight, -halfDepth, 1.0F }, 0 },

    { { halfDepth, -halfWidth, -halfHeight, 1.0F }, 0 },
    { { halfDepth, -halfWidth, halfHeight, 1.0F }, 0 },
    { { halfDepth, halfWidth, -halfHeight, 1.0F }, 0 },
    { { halfDepth, halfWidth, halfHeight, 1.0F }, 0 },

    { { -halfDepth, -halfWidth, -halfHeight, 1.0F }, 0 },
    { { -halfDepth, halfWidth, -halfHeight, 1.0F }, 0 },
    { { -halfDepth, -halfWidth, halfHeight, 1.0F }, 0 },
    { { -halfDepth, halfWidth, halfHeight, 1.0F }, 0 },

    { { -halfWidth, -halfDepth, -halfHeight, 1.0F }, 0 },
    { { -halfWidth, -halfDepth, halfHeight, 1.0F }, 0 },
    { { halfWidth, -halfDepth, -halfHeight, 1.0F }, 0 },
    { { halfWidth, -halfDepth, halfHeight, 1.0F }, 0 },

    { { -halfWidth, halfDepth, -halfHeight, 1.0F }, 0 },
    { { halfWidth, halfDepth, -halfHeight, 1.0F }, 0 },
    { { -halfWidth, halfDepth, halfHeight, 1.0F }, 0 },
    { { halfWidth, halfDepth, halfHeight, 1.0F }, 0 },
  };
  float generated_uv[][2] = {
    { 0.0F, 0.0F }, { 0.0F, 1.0F }, { 1.0F, 0.0F }, { 1.0F, 1.0F },
    { 0.0F, 0.0F }, { 1.0F, 0.0F }, { 0.0F, 1.0F }, { 1.0F, 1.0F },
    { 0.0F, 0.0F }, { 0.0F, 1.0F }, { 1.0F, 0.0F }, { 1.0F, 1.0F },
    { 0.0F, 0.0F }, { 1.0F, 0.0F }, { 0.0F, 1.0F }, { 1.0F, 1.0F },
    { 0.0F, 0.0F }, { 0.0F, 1.0F }, { 1.0F, 0.0F }, { 1.0F, 1.0F },
    { 0.0F, 0.0F }, { 1.0F, 0.0F }, { 0.0F, 1.0F }, { 1.0F, 1.0F },
  };
  static unsigned long generated_uvIndices[] = {
    0, 1, 2, 1, 3, 2,
    4, 5, 6, 5, 7, 6,
    8, 9, 10, 9, 11, 10,
    12, 13, 14, 13, 15, 14,
    16, 17, 18, 17, 19, 18,
    20, 21, 22, 21, 23, 22,
  };
  for(i = 0;i < 36;i++) {
    unsigned long *index = malloc(sizeof(unsigned long));
    unsigned long *uvIndex = malloc(sizeof(unsigned long));
    *index = generated_indices[i];
    *uvIndex = generated_uvIndices[i];
    push(indices, index);
    push(uvIndices, uvIndex);
  }
  for(i = 0;i < 24;i++) {
    Vertex *vertex = malloc(sizeof(Vertex));
    float *coords = malloc(2 * sizeof(float));
    *vertex = generated_vertices[i];
    coords[0] = generated_uv[i][0];
    coords[1] = generated_uv[i][1];
    push(vertices, vertex);
    push(uv, coords);
  }
}

size_t getUntil(char *string, char separator, size_t index, char *out, size_t out_size) {
  size_t i = 0;
  if(string[index] == '\0') {
    out[0] = '\0';
    return 0;
  }
  for(;string[index] != '\0';index++) {
    if(string[index] != separator) break;
  }
  for(;string[index] != '\0';index++) {
    if(string[index] == separator || string[index] == '\n') {
      out[i] = '\0';
      break;
    }
    if(i < out_size - 1) {
      out[i] = string[index];
      i += 1;
    }
  }
  return index + 1;
}

void readObj(char *filename, Vector *indices, Vector *vertices, Vector *uv, Vector *uvIndices) {
  FILE *file;
  char buffer[OBJ_LINE_BUFFER_SIZE];
  char temp[OBJ_WORD_BUFFER_SIZE];
  size_t line = 1;
  *vertices = initVector();
  *indices = initVector();
  *uv = initVector();
  *uvIndices = initVector();
  if(fopen_s(&file, filename, "r")) {
    fputs("File not found.", stderr);
    fclose(file);
    return;
  }
  while(fgets(buffer, OBJ_LINE_BUFFER_SIZE, file)) {
    int i;
    size_t index = 0;
    size_t old_index = 0;
    index = getUntil(buffer, ' ', index, temp, OBJ_WORD_BUFFER_SIZE);
    if(strcmp(temp, "v") == 0) {
      Vertex *vertex = calloc(sizeof(Vertex), 1);
      if(vertex == NULL) {
        fputs("readObj: Memory allocation failed.", stderr);
        fclose(file);
        return;
      }
      for(i = 0;i < 4;i++) {
        old_index = index;
        index = getUntil(buffer, ' ', index, temp, OBJ_WORD_BUFFER_SIZE);
        if(temp[0] == '\0') {
          if(i == 3) {
            vertex->components[3] = 1.0F;
          } else {
            fprintf(stderr, "readObj: Vertex component %d does not found. (%zu, %zu)", i, line, old_index);
            fclose(file);
            return;
          }
        } else {
          vertex->components[i] = (float)atof(temp);
        }
      }
      push(vertices, vertex);
    } else if(strcmp(temp, "vt") == 0) {
      float *coords = malloc(2 * sizeof(float));
      if(coords == NULL) {
        fputs("readObj: Memory allocation failed.", stderr);
        fclose(file);
        return;
      }
      for(i = 0;i < 2;i++) {
        old_index = index;
        index = getUntil(buffer, ' ', index, temp, OBJ_WORD_BUFFER_SIZE);
        if(temp[0] == '\0') {
          if(i != 0) {
            coords[1] = 0.0F;
          } else {
            fprintf(stderr, "readObj: Terxture cordinates' component %d does not found. (%zu, %zu)", i, line, old_index);
            fclose(file);
            return;
          }
        } else {
          if(i == 1) {
            coords[i] = 1.0F - (float)atof(temp);
          } else {
            coords[i] = (float)atof(temp);
          }
        }
      }
      push(uv, coords);
    } else if(strcmp(temp, "f") == 0) {
      unsigned long faceIndices[3];
      unsigned long faceUVIndices[3];
      for(i = 0;i < 3;i++) {
        old_index = index;
        index = getUntil(buffer, ' ', index, temp, OBJ_WORD_BUFFER_SIZE);
        if(temp[0] == '\0') {
          fprintf(stderr, "readObj: Vertex component %d does not found. (%zu, %zu)", i, line, old_index);
          fclose(file);
          return;
        } else {
          size_t index2 = 0;
          char temp2[OBJ_WORD_BUFFER_SIZE];
          index2 = getUntil(temp, '/', index2, temp2, OBJ_WORD_BUFFER_SIZE);
          faceIndices[i] = atoi(temp2);
          if(faceIndices[i] < 0) {
            faceIndices[i] += vertices->length;
          } else {
            faceIndices[i] -= 1;
          }
          index2 = getUntil(temp, '/', index2, temp2, OBJ_WORD_BUFFER_SIZE);
          if(temp2[0] == '\0') {
            faceUVIndices[i] = 0;
          } else {
            faceUVIndices[i] = atoi(temp2);
            if(faceUVIndices[i] < 0) {
              faceUVIndices[i] += uv->length;
            } else {
              faceUVIndices[i] -= 1;
            }
          }
        }
      }
      for(i = 2;i >= 0;i--) {
        unsigned long *faceIndex = malloc(sizeof(unsigned long));
        unsigned long *faceUVIndex = malloc(sizeof(unsigned long));
        if(faceIndex == NULL || faceUVIndex == NULL) {
          fputs("readObj: Memory allocation failed.", stderr);
          fclose(file);
          return;
        }
        *faceIndex = faceIndices[i];
        *faceUVIndex = faceUVIndices[i];
        push(indices, faceIndex);
        push(uvIndices, faceUVIndex);
      }
    } else if(temp[0] != '\0' && temp[0] != '#' && strcmp(temp, "vn") != 0 &&
              strcmp(temp, "vp") != 0 && strcmp(temp, "l") != 0 && strcmp(temp, "mtllib") != 0 &&
              strcmp(temp, "o") != 0 && strcmp(temp, "usemtl") != 0 && strcmp(temp, "s") != 0) {
      fprintf(stderr, "readObj: Unexpected word '%s' (%zu)", temp, line);
      fclose(file);
      return;
    }
    line += 1;
  }
  fclose(file);
}