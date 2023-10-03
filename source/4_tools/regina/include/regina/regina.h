namespace rex
{
  bool initialize();
  void update();
  void shutdown();

  // we put the generation of the app creation params into a separate function
  // so that auto tests can always start of with the same settings as regina and change them where needed
  struct ApplicationCreationParams create_regina_app_creation_params(struct PlatformCreationParams&& platformParams);
} // namespace rex