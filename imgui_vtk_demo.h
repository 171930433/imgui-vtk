#pragma once
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkContourFilter.h>
#include <vtkMath.h>
#include <vtkNamedColors.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkShortArray.h>
#include <vtkStructuredPoints.h>
//
#include <vtkCylinderSource.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkConeSource.h>
#include <vtkPlaneSource.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkAssembly.h>

// void SetupScene(vtkSmartPointer<vtkRenderer> renderer);
// void Assemblies(vtkSmartPointer<vtkRenderer> renderer);

static vtkSmartPointer<vtkActor> SetupDemoPipeline()
{
  double Pr = 10.0; // The Lorenz parameters
  double b = 2.667;
  double r = 28.0;
  double x, y, z;       // starting (and current) x, y, z
  double h = 0.01;      // integration step size
  int resolution = 200; // slice resolution
  int iter = 10000000;  // number of iterations
  double xmin = -30.0;  // x, y, z range for voxels
  double xmax = 30.0;
  double ymin = -30.0;
  double ymax = 30.0;
  double zmin = -10.0;
  double zmax = 60.0;

  // take a stab at an integration step size
  auto xIncr = resolution / (xmax - xmin);
  auto yIncr = resolution / (ymax - ymin);
  auto zIncr = resolution / (zmax - zmin);

  printf("The Lorenz Attractor\n");
  printf("  Pr = %f\n", Pr);
  printf("  b = %f\n", b);
  printf("  r = %f\n", r);
  printf("  integration step size = %f\n", h);
  printf("  slice resolution = %d\n", resolution);
  printf("  # of iterations = %d\n", iter);
  printf("  specified range:\n");
  printf("      x: %f, %f\n", xmin, xmax);
  printf("      y: %f, %f\n", ymin, ymax);
  printf("      z: %f, %f\n", zmin, zmax);

  x = vtkMath::Random(xmin, xmax);
  y = vtkMath::Random(ymin, ymax);
  z = vtkMath::Random(zmin, zmax);
  printf("  starting at %f, %f, %f\n", x, y, z);

  // allocate memory for the slices
  auto sliceSize = resolution * resolution;
  auto numPts = sliceSize * resolution;
  auto scalars =
      vtkSmartPointer<vtkShortArray>::New();
  auto s = scalars->WritePointer(0, numPts);
  for (auto i = 0; i < numPts; i++)
  {
    s[i] = 0;
  }
  for (auto j = 0; j < iter; j++)
  {
    // integrate to next time step
    auto xx = x + h * Pr * (y - x);
    auto yy = y + h * (x * (r - z) - y);
    auto zz = z + h * (x * y - (b * z));

    x = xx;
    y = yy;
    z = zz;

    // calculate voxel index
    if (x < xmax && x > xmin && y < ymax && y > ymin && z < zmax && z > zmin)
    {
      auto xxx = static_cast<short>(static_cast<double>(xx - xmin) * xIncr);
      auto yyy = static_cast<short>(static_cast<double>(yy - ymin) * yIncr);
      auto zzz = static_cast<short>(static_cast<double>(zz - zmin) * zIncr);
      auto index = xxx + yyy * resolution + zzz * sliceSize;
      s[index] += 1;
    }
  }

  auto colors =
      vtkSmartPointer<vtkNamedColors>::New();

  auto volume =
      vtkSmartPointer<vtkStructuredPoints>::New();
  volume->GetPointData()->SetScalars(scalars);
  volume->SetDimensions(resolution, resolution, resolution);
  volume->SetOrigin(xmin, ymin, zmin);
  volume->SetSpacing((xmax - xmin) / resolution, (ymax - ymin) / resolution,
                     (zmax - zmin) / resolution);

  printf("  contouring...\n");

  // create iso-surface
  auto contour =
      vtkSmartPointer<vtkContourFilter>::New();
  contour->SetInputData(volume);
  contour->SetValue(0, 50);

  // create mapper
  auto mapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(contour->GetOutputPort());
  mapper->ScalarVisibilityOff();

  // create actor
  auto actor =
      vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
#if VTK_MAJOR_VERSION >= 9
  actor->GetProperty()->SetColor(colors->GetColor3d("PaleTurquoise").GetData());
#endif

  return actor;
}

static void Assemblies(vtkSmartPointer<vtkRenderer> renderer)
{
  // sphere
  vtkNew<vtkSphereSource> sphere;
  vtkNew<vtkPolyDataMapper> sphere_mapper;
  sphere_mapper->SetInputConnection(sphere->GetOutputPort());
  vtkNew<vtkActor> sphere_actor;
  sphere_actor->SetMapper(sphere_mapper);
  // sphere_actor->SetOrigin(2, 1, 3);
  // sphere_actor->RotateY(6);
  sphere_actor->SetPosition(5, 1, -1);
  sphere_actor->GetProperty()->SetColor(1, 0, 1);

  // cube
  vtkNew<vtkCubeSource> cube;
  vtkNew<vtkPolyDataMapper> cube_mapper;
  cube_mapper->SetInputConnection(cube->GetOutputPort());
  vtkNew<vtkActor> cube_actor;
  cube_actor->SetMapper(cube_mapper);
  cube_actor->SetPosition(5, 1, 0);
  cube_actor->GetProperty()->SetColor(0, 0, 1);

  // cone
  vtkNew<vtkConeSource> cone;
  vtkNew<vtkPolyDataMapper> cone_mapper;
  cone_mapper->SetInputConnection(cone->GetOutputPort());
  vtkNew<vtkActor> cone_actor;
  cone_actor->SetMapper(cone_mapper);
  cone_actor->SetPosition(5, 1, 2);
  cone_actor->GetProperty()->SetColor(0, 1, 0);
  cone_actor->RotateY(-90);

  // cylinder
  vtkNew<vtkCylinderSource> cylinder;
  vtkNew<vtkPolyDataMapper> cylinder_mapper;
  cylinder_mapper->SetInputConnection(cylinder->GetOutputPort());
  vtkNew<vtkActor> cylinder_actor;
  cylinder_actor->SetMapper(cylinder_mapper);
  cylinder_actor->SetPosition(5, 1, 1);
  cylinder_actor->RotateX(90);
  cylinder_actor->GetProperty()->SetColor(1, 0, 0);

  // assembly
  vtkNew<vtkAssembly> assembly;
  assembly->AddPart(sphere_actor);
  assembly->AddPart(cube_actor);
  assembly->AddPart(cone_actor);
  assembly->AddPart(cylinder_actor);

  assembly->SetOrigin(5, 10, 15);
  assembly->AddPosition(5, 0, 0);
  assembly->RotateX(15);

  renderer->AddActor(assembly);
  renderer->AddActor(cone_actor);
}

static void SetupScene(vtkSmartPointer<vtkRenderer> renderer)
{
  vtkNew<vtkAxesActor> axes;
  axes->GetXAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
  axes->GetYAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();
  axes->GetZAxisCaptionActor2D()->GetTextActor()->SetTextScaleModeToNone();

  renderer->AddActor(axes);

  // fps
  vtkNew<vtkTextActor> text_actor;
  text_actor->SetPosition(10, 10);
  text_actor->GetTextProperty()->SetColor(1, 1, 1);
  text_actor->SetInput("FPS:");
  renderer->AddActor2D(text_actor);

  // xyplane
  vtkNew<vtkPlaneSource> plane_xy;
  plane_xy->SetNormal(0, 0, 1);
  plane_xy->SetCenter(0, 0, 0);

  vtkNew<vtkPolyDataMapper> plane_mapper;
  plane_mapper->SetInputConnection(plane_xy->GetOutputPort());

  vtkNew<vtkActor> plane_actor;
  plane_actor->SetMapper(plane_mapper);
  plane_actor->GetProperty()->SetOpacity(0.2);
  plane_actor->SetScale(10, 10, 1);

  renderer->AddActor(plane_actor);

  Assemblies(renderer);
}
