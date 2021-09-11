#include <stdio.h>

int
main() {

    float cameraX2 = 0.0f;
    float cameraXInc = 1.0f / (float)250;
    printf("CameraX: ");
    for(int x = 0; x < 250; ++x)
    {
        float cameraX = 2 * x / (float)250 - 1;
        cameraX2 = 2 * x * cameraXInc - 1;
        printf("%f, %f\n", cameraX, cameraX2);
    }
}
