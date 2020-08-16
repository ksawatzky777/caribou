[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 50
  ny = 50
  nz = 50
  xmin = 0.0
  xmax = 465000.0
  ymin = 0.0
  ymax = 465000.0
  zmin = 0.0
  zmax = 11322.0
[]

[Variables]
  [./cs137]
    order = FIRST
    family = LAGRANGE

    [./InitialCondition]
      type = ConstantIC
      value = 0.0
    [../]
  [../]
[]

[Kernels]
  [./diff]
    type = STDiffusion
    variable = cs137
  [../]

  [./advc]
    type = STAdvection
    variable = cs137
    upwinding_type = full
  [../]

  [./time]
    type = STTimeDerivative
    variable = cs137
  [../]

  [./decay]
    type = SpeciesDecay
    variable = cs137
  [../]

  [./settling]
    type = SpeciesSettling
    variable = cs137
  [../]
[]

[DiracKernels]
  [./srce]
    variable = cs137
    type = PieceWisePointSource
    rates = '1.1e11'
    activation_times = '0.0'
    point = '232500.0 232500.0 0.0'
  [../]
[]

[BCs]
  [./open]
    type = MaterialOutflowBC
    variable = cs137
    boundary = '0 2 3 4 5'
  [../]
  [./bott]
    type = DryDepositionBC
    variable = cs137
    boundary = '1'
    dry_deposition_velocity = 0.001
  [../]
[]

[Materials]
  [./plume]
    type = GenericCaribouMaterial
    u_file_name = u.csv
    v_file_name = v.csv
    w_file_name = w.csv
    dim_file_name = coords.csv
    diffusivity = 1.0
    decay_constant = 7.285e-10
    settling_velocity = -0.002
  [../]
[../]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  num_steps = 480
  dt = 60
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]
