

select * from Neurons where NeuronGroupId = 185 order by x, y, z asc;

select * from NeuronGroups where NeuronGroupId in (185, 184);

select * from NeuronGroups where NetworkId >= 382 order by NeuronGroupId asc;

select * from ConnectionGroups where NetworkId >= 382 order by ConnectionGroupId asc;


select * from Connections where ConnectionGroupId = 304;

update Connections set Weight = 0.35 where FromNeuronID = 31957 and ToNeuronID = 31961;

update Connections set Delay = 4, Weight = 0.35 where FromNeuronID = 31953 and ToNeuronID = 31957;
