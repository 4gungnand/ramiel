#include <GL/glut.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <array>

struct Vertex {
    float x, y, z;
};

struct Polygon {
    std::vector<int> vertices;
};

std::vector<Vertex> verticesTop;
std::vector<Polygon> polygonsTop;
std::vector<Vertex> verticesBottom;
std::vector<Polygon> polygonsBottom;

// Variables for animation
float translationZ = -10.0f;
float translationSpeed = 0.05f;
float splitTranslation = 0.0f;
float splitSpeed = 0.005f;
float rotationAngleTop = 0.0f;
float rotationAngleBottom = 0.0f;
float rotationSpeed = 0.5f;

// Function for drawing the object with offset
void drawObject(const std::vector<Vertex>& vertices, const std::vector<Polygon>& polygons, float offset) {
    // Define the fixed colors for each polygon
    std::vector<std::array<float, 4>> polygonColors = {
        {101.0f / 255.0f, 146.0f / 255.0f, 194.0f / 255.0f, 1.0f},    // Blue-gray
        {41.0f / 255.0f, 65.0f / 255.0f, 169.0f / 255.0f, 1.0f},      // Violet-blue
        {18.0f / 255.0f, 16.0f / 255.0f, 16.0f / 255.0f, 1.0f},       // Night
        {45.0f / 255.0f, 69.0f / 255.0f, 73.0f / 255.0f, 1.0f},       // Dark slate gray
        {4.0f / 255.0f, 18.0f / 255.0f, 117.0f / 255.0f, 1.0f}        // Federal blue
    };

    // Draw the filled object
    glBegin(GL_QUADS);  // Use GL_QUADS for square polygons
    int colorIndex = 0;
    for (const Polygon& polygon : polygons) {
        // Get the color for the current polygon
        const std::array<float, 4>& color = polygonColors[colorIndex % polygonColors.size()];
        glColor4f(color[0], color[1], color[2], color[3]);

        // Draw the polygon
        int numVertices = polygon.vertices.size();
        for (int i = 0; i < numVertices; ++i) {
            const Vertex& vertex = vertices[polygon.vertices[i] - 1];
            glVertex3f(vertex.x, vertex.y + offset/5.0f, vertex.z);
        }

        colorIndex++;
    }
    glEnd();
}


// Function for reading object data from a text file
void readObjectData(const std::string& filename, std::vector<Vertex>& vertices, std::vector<Polygon>& polygons) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    int numVertices, numPolygons;
    file >> numVertices >> numPolygons;

    // Read the vertices coordinate
    vertices.resize(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        file >> vertices[i].x >> vertices[i].y >> vertices[i].z;
    }

    // Read the polygons
    polygons.resize(numPolygons);
    for (int i = 0; i < numPolygons; ++i) {
        int numVertices;
        file >> numVertices;

        Polygon polygon;
        polygon.vertices.resize(numVertices);
        for (int j = 0; j < numVertices; ++j) {
            file >> polygon.vertices[j];
        }

        polygons[i] = polygon;
    }

    file.close();
}

// Function for displaying the scene
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Translate the object
    glTranslatef(0.0f, 0.0f, translationZ);

    // Rotate the entire object
    glRotatef(rotationAngleTop, 0.0f, 1.0f, 0.0f);

    // Draw the top pyramid
    drawObject(verticesTop, polygonsTop, splitTranslation);

    // Save the current matrix
    glPushMatrix();

    // Rotate the bottom pyramid
    glRotatef(rotationAngleBottom, 0.0f, -1.0f, 0.0f);

    // Draw the bottom pyramid
    drawObject(verticesBottom, polygonsBottom, -splitTranslation);

    // Restore the matrix
    glPopMatrix();

    glutSwapBuffers();
}

// Function for updating the scene
void update(int value) {
    // Update the translation along the z-axis
    translationZ += translationSpeed;

    // Reverse the translation direction if reaching the desired position
    if (translationZ > 0.0f || translationZ < -20.0f)
        translationSpeed *= -1.0f;

    // Update the split translation
    splitTranslation += splitSpeed;

    // Reverse the split translation direction if reaching the desired position
    if (splitTranslation > 0.5f || splitTranslation < 0.0f)
        splitSpeed *= -1.0f;

    // Update the rotation angles
    rotationAngleTop += rotationSpeed;
    rotationAngleBottom -= rotationSpeed;

    // Redisplay the scene
    glutPostRedisplay();

    // Call update function again after 16 milliseconds (60 FPS)
    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv) {
    // Read the object data from the text files
    readObjectData("t.txt", verticesTop, polygonsTop);
    readObjectData("b.txt", verticesBottom, polygonsBottom);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Splitting Octahedron");

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 100.0);

    glutDisplayFunc(display);
    glutTimerFunc(0, update, 0);

    glutMainLoop();

    return 0;
}
