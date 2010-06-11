package granDT;

import org.jgap.Configuration;
import org.jgap.FitnessFunction;
import org.jgap.Genotype;
import org.jgap.InvalidConfigurationException;
import org.jgap.impl.DefaultConfiguration;

public class GranDT {


	private static final int MAX_ALLOWED_EVOLUTIONS = 100;

	public static void main(String[] args) throws InvalidConfigurationException {
		  // most common settings.
		  // -------------------------------------------------------------
		  Configuration conf = new DefaultConfiguration();

		  //funcion de aptitud
		  FitnessFunction f = new FuncionAptitudEquipo();
		  conf.setFitnessFunction( f );

		  //cromosoma a utilizar
		  conf.setSampleChromosome( new EquipoCromosoma(conf) );

		  // Finally, we need to tell the Configuration object how many
		  // Chromosomes we want in our population. The more Chromosomes,
		  // the larger the number of potential solutions (which is good
		  // for finding the answer), but the longer it will take to evolve
		  // the population each round. We'll set the population size to
		  // 500 here.
		  // --------------------------------------------------------------
		  conf.setPopulationSize( 2 );
	
		  //poblacion inicial
		  Genotype population = Genotype.randomInitialGenotype( conf );

		  for( int i = 0; i < MAX_ALLOWED_EVOLUTIONS; i++ )
		  {
		      population.evolve();
		  }
		  
		  System.out.println("hola mundo");

	}

}
