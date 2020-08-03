[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 50
  ny = 50
  nz = 50
  xmin = 0.0
  xmax = 124000.0
  ymin = 0.0
  ymax = 124000.0
  zmin = 0.0
  zmax = 11539.0
[]

[Variables]
  [./concentration]
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
    variable = concentration
  [../]

  [./advc]
    type = STAdvection
    variable = concentration
    upwinding_type = full
  [../]

  [./time]
    type = STTimeDerivative
    variable = concentration
  [../]
[]

[DiracKernels]
  [./srce]
    variable = concentration
    type = TimedPointSource
    rate = 100.0
    point = '62000.0 62000.0 0.0'
    deactivation_time = 50.0
  [../]
[]

[BCs]
  [./open]
    type = MaterialOutflowBC
    variable = concentration
    boundary = '0 2 3 4 5'
  [../]
  [./bott]
    type = DryDepositionBC
    variable = concentration
    boundary = '1'
    dry_deposition_velocity = 1.0
  [../]
[]

[Materials]
  [./test]
    type = STMaterial
    u_file_name = u.csv
    v_file_name = v.csv
    w_file_name = w.csv
    dim_file_name = coords.csv
    diffusivity = 1.0
    #time_dependance = true
    outputs = exodus
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  num_steps = 100
  dt = 1
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]
