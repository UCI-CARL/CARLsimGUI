
/*---------------------------- Add Neuron Types ---------------------------- */
/*	SQL to add the current neuron types to the Network Database.*/
/*-----------------------------------------------------------------------------*/

/* Add an entry for excitatory Izhikevich neurons to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (1, "Izhikevich Excitatory Neuron", "IzhikevichExcitatoryNeuronParameters", "");


/* Add an entry for inhibitory Izhikevich neurons to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (2, "Izhikevich Inhibitory Neuron", "IzhikevichInhibitoryNeuronParameters", "");

/* Add an entry for the Weightless neurons to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (3, "Weightless Neuron", "WeightlessNeuronParameters", "");


/* featInputNeurons (LN20201025) */

/* Add an entry for dopaminergic neurons to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (4, "Dopaminergic Neuron", "DopaminergicNeuronParameters", "");

/* Add an entry for excitatory custom activated neurons to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (5, "Custom Excitatory Neuron", "CustomExcitatoryNeuronParameters", "");

/* Add an entry for inhibitory custom activated neuron to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (6, "Custom Inhibitory Neuron", "CustomInhibitoryNeuronParameters", "");

/* Add an entry for excitatory neurons activated by a Poisson process to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (7, "Poisson Excitatory Neuron", "PoissonExcitatoryNeuronParameters", "");

/* Add an entry for inhibitory neurons activated by a Poisson process to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (8, "Poisson Inhibitory Neuron", "PoissonInhibitoryNeuronParameters", "");



/* Add table for Izhikevich Neuron parameters for an excitatory neuron. 
	These are described in E. M. Izhikevich. Simple model of spiking neurons. IEEE Trans. Neural
	Networks, 14:1569{1572, 2003.*/
CREATE TABLE IzhikevichExcitatoryNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	
	a DOUBLE DEFAULT 0.02 COMMENT 'Time scale of the recovery variable, u.',
	b DOUBLE DEFAULT 0.2 COMMENT 'Sensitivity to sub-threshold fluctutations in the membrane potential, v.',
	c_1 DOUBLE DEFAULT 15.0 COMMENT 'After-spike reset value of the membrane potential, v.\nc is calculated using the equation c = v + c_1 * random1 * random2',
	d_1 DOUBLE DEFAULT 8.0 COMMENT 'After-spike reset of the recovery variable, u.\nd is calculated using the equation d = d_1 - d_2 * random1 * random2',
	d_2 DOUBLE DEFAULT 6.0 COMMENT 'After-spike reset of the recovery variable, u.\nd is calculated using the equation d = d_1 - d_2 * random1 * random2',
	v DOUBLE DEFAULT -65.0 COMMENT 'Initial value for the membrane potential.',
	sigma DOUBLE DEFAULT 0.0 COMMENT 'Parameter for a random gaussian per-neuron process, which generates random input current drawn from an N(0,\a sigma) distribution. If set to zero no random input current will be generated.',
	seed DOUBLE DEFAULT 45 COMMENT 'Seed for the random number generator. Will be rounded to the nearest integer. If the seed is <=0 the random number generator will be seeded using the time.',
	
	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;

/* Add table for Izhikevich Neuron parameters for an inhibitory neuron. 
	These are described in E. M. Izhikevich. Simple model of spiking neurons. IEEE Trans. Neural
	Networks, 14:1569{1572, 2003.*/
CREATE TABLE IzhikevichInhibitoryNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	
	a_1 DOUBLE DEFAULT 0.02 COMMENT 'Time scale of the recovery variable, u. \na_1 is the first parameter that is used to generate a in equation a = a_1 + a_2 * random.',
	a_2 DOUBLE DEFAULT 0.08 COMMENT 'Time scale of the recovery variable, u. a_2 is the second parameter that is used to generate a in equation a = a_1 + a_2 * random.',
	b_1 DOUBLE DEFAULT 0.25 COMMENT 'Sensitivity to sub-threshold fluctutations in the membrane potential, v. b_1 is the first parameter that is used to generate a in equation b = b_1 - b_2 * random.',
	b_2 DOUBLE DEFAULT 0.05 COMMENT 'Sensitivity to sub-threshold fluctutations in the membrane potential, v. b_1 is the first parameter that is used to generate a in equation b = b_1 - b_2 * random.',
	d DOUBLE DEFAULT 2.0 COMMENT 'After-spike reset of the recovery variable, u.',
	v DOUBLE DEFAULT -65.0 COMMENT 'Initial value for the membrane potential.',
	sigma DOUBLE DEFAULT 0.0 COMMENT 'Parameter for a random gaussian per-neuron process, which generates random input current drawn from an N(0,\a sigma) distribution. If set to zero no random input current will be generated.',
	seed DOUBLE DEFAULT 45 COMMENT 'Seed for the random number generator. Will be rounded to the nearest integer. If the seed is <=0 the random number generator will be seeded using the time.',
	
	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Add table for Weightless Neuron parameters
	Table containing the parameters for the Spike time dependent plasticity Neuron type 1
	Each neuron group created with this type will have an entry in this table.*/
CREATE TABLE WeightlessNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	Generalization DOUBLE DEFAULT 1.0 COMMENT 'Degree of match using hamming distance with incoming pattern',

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* featInputNeurons (LN20201025) */

/* Add table for parameters of an Dopminergic Neuron. */
CREATE TABLE DopaminergicNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	
	a DOUBLE DEFAULT 0.02 COMMENT 'Time scale of the recovery variable, u.',
	b DOUBLE DEFAULT 0.2 COMMENT 'Sensitivity to sub-threshold fluctutations in the membrane potential, v.',
	c_1 DOUBLE DEFAULT 15.0 COMMENT 'After-spike reset value of the membrane potential, v.\nc is calculated using the equation c = v + c_1 * random1 * random2',
	d_1 DOUBLE DEFAULT 8.0 COMMENT 'After-spike reset of the recovery variable, u.\nd is calculated using the equation d = d_1 - d_2 * random1 * random2',
	d_2 DOUBLE DEFAULT 6.0 COMMENT 'After-spike reset of the recovery variable, u.\nd is calculated using the equation d = d_1 - d_2 * random1 * random2',
	v DOUBLE DEFAULT -65.0 COMMENT 'Initial value for the membrane potential.',
	sigma DOUBLE DEFAULT 0.0 COMMENT 'Parameter for a random gaussian per-neuron process, which generates random input current drawn from an N(0,\a sigma) distribution. If set to zero no random input current will be generated.',
	seed DOUBLE DEFAULT 45 COMMENT 'Seed for the random number generator. Will be rounded to the nearest integer. If the seed is <=0 the random number generator will be seeded using the time.',
	
  da_baseline DOUBLE DEFAULT 0.0 COMMENT 'baseline of the DA level', 
  da_tau DOUBLE DEFAULT 0.0 COMMENT 'decay rate of the DA concentration', 

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Add table for parameters of Custom Excitatory Neurons. 
  */
CREATE TABLE CustomExcitatoryNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,

  /*reserved, no generic sofar, its custom..*/

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;

 
/* Add table for parameters of Custom Inhibitory Neurons. 
  LN20201025
  */
CREATE TABLE CustomInhibitoryNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,

  /*reserved, no generic sofar, its custom..*/

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Add table for parameters of Poisson like neurons.  
   LN20201025
  */
CREATE TABLE PoissonExcitatoryNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,

  Generator SMALLINT DEFAULT 1 COMMENT '0 no generator = see Custom, 1 Poisson, 2 Periodical, mean = actual , see 6.1', 
  SpikeAtZero BOOL DEFAULT false COMMENT ' 6.1.2',
	Rate DOUBLE DEFAULT 10.0 COMMENT 'mean firing rate in Hz  6.1.1',
  Rates TEXT COMMENT ' blank separated list of rates, length > neuron group trunc, < => repeated filled up',

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Add table for parameters of Poisson like neurons.  
   LN20201025
  */
CREATE TABLE PoissonInhibitoryNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,

  Generator SMALLINT DEFAULT 1 COMMENT '0 no generator = see Custom, 1 Poisson, 2 Periodical, mean = actual , see 6.1', 
  SpikeAtZero BOOL DEFAULT false COMMENT ' 6.1.2',
	Rate DOUBLE DEFAULT 10.0 COMMENT 'mean firing rate in Hz  6.1.1',
  Rates TEXT COMMENT ' blank separated list of rates, length > neuron group trunc, < => repeated filled up',

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;