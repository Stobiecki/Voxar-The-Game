#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <functional>
#include <sstream>
#include <memory>

#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <gtc/type_ptr.hpp>

#include "Model.h"

class Crosshair
{
private:

public:
    void InitCrossroad(ProgramID& program);

    std::vector<glm::vec2> vert;
    std::vector<glm::vec2> text;
    bool visable = false;
    int texWidth;
    int texHeigh;
    int texN;
    unsigned char* texData;
};

class GUI
{
public:
    static void InitGuiInfo();
    static void RenderAll();
    static void freeData();

    ProgramID program;
    Crosshair crosshair;
    static GUI gui;
};