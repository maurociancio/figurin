package granDT;

import org.jgap.Chromosome;
import org.jgap.Configuration;
import org.jgap.Gene;
import org.jgap.InvalidConfigurationException;
import org.jgap.impl.IntegerGene;

@SuppressWarnings("serial")
public class EquipoCromosoma extends Chromosome {

	public EquipoCromosoma(Configuration conf)
			throws InvalidConfigurationException {
		super(conf);

		Gene[] jugadores = new Gene[4];

		jugadores[0] = new JugadorGen();
		jugadores[0] = new IntegerGene(conf, 0, 3); // Quarters
		jugadores[1] = new IntegerGene(conf, 0, 2); // Dimes
		jugadores[2] = new IntegerGene(conf, 0, 1); // Nickels
		jugadores[3] = new IntegerGene(conf, 0, 4); // Pennies

		this.setGenes(jugadores);
	}

}
