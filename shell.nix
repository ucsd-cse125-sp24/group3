{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  name = "group3";
  buildInputs = with pkgs; [
    cmake
    gnumake
    gcc13

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

    clang-tools_13
  ];
  nativeBuildInputs = with pkgs; [
        pkg-config
  ];
  shellHook = ''
      LD_LIBRARY_PATH="/run/opengl-driver/lib:/run/opengl-driver-32/lib";
  '';
}
