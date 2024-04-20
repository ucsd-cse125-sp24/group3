{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  name = "group3";
  buildInputs = with pkgs; [
    cmake
    gnumake
    gcc13
    gdb

    wayland
    wayland-scanner
    libxkbcommon
    libffi
    xorg.libX11
    xorg.libXrandr
    xorg.libXinerama
    xorg.libXcursor
    xorg.libXi
    
    glfw
    glm

    freeglut
    libGLU

    doxygen
    clang-tools_14
  ];
  nativeBuildInputs = with pkgs; [
        pkg-config
  ];
  LD_LIBRARY_PATH = with pkgs; "${freeglut}/lib:${libGLU}/lib";
  shellHook = ''
      LD_LIBRARY_PATH="/run/opengl-driver/lib:/run/opengl-driver-32/lib";
  '';
}
