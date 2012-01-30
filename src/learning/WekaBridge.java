import java.io.*;
import java.util.*;
import weka.core.*;
import weka.classifiers.Classifier;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.ByteOrder;

public class WekaBridge {
	static Classifier classifier;

  static private native void init(String memSegName, int numFeatures, int numClasses);
  static private native byte readCommand(double[] features,double[] weight);
  static private native void writeDistr(double[] distr);
  static private native void send();


  static {
    System.loadLibrary("WekaBridge");
  }

  public static void main(String [] argv) {

    try {
      String memSegName = argv[0];
      String dataName = argv[1];
      String classifierName = argv[2];
      String[] opts = new String[argv.length-3];
      for (int i = 0; i < opts.length; i++) {
        opts[i] = argv[i+3];
      }

      weka.core.WekaPackageManager.loadPackages(false, false);
      // get the data groups set up
      Instances trainData = new Instances(new BufferedReader(new FileReader(dataName)));
      trainData.setClassIndex(trainData.numAttributes()-1);
      Instances testData = new Instances(trainData,0);
      // initialize the memory for communication
      byte commandByte = '\0';
      Instance inst;
      init(memSegName,trainData.numAttributes(),trainData.numClasses());
      // create the classifier
      classifier = (Classifier)Utils.forName(Classifier.class,classifierName,opts);
      //classifier.buildClassifier(trainData);
      // wait for commands
      while (true) {
        double[] features = new double[trainData.numAttributes()];
        double[] weight = new double[1];
        double[] distr;
        commandByte = readCommand(features,weight);
        if (commandByte == 'e')
          break;
        switch (commandByte) {
          case 't':
            //System.out.format("TRAINING %d%n",trainData.numInstances());
            //for (int i = 0; i < trainData.numInstances(); i++) {
              //System.out.println(trainData.instance(i).toString());
            //}
            classifier.buildClassifier(trainData);
            break;
          case 'c':
            inst = new DenseInstance(weight[0],features);
            testData.add(inst);
            distr = classifier.distributionForInstance(testData.lastInstance());
            writeDistr(distr);
            testData.delete();
            break;
          case 'a':
            inst = new DenseInstance(weight[0],features);
            //System.err.format("ADDING: %s {%f}%n",inst.toString(),inst.weight());
            //System.out.format("compat: %b%n",trainData.checkInstance(inst));
            //System.out.println(trainData.toString());
            trainData.add(inst);
            //System.out.format("ADDING2: %s {%f}%n",trainData.instance(0).toString(),trainData.instance(0).weight());
            break;
          case 'p':
            System.out.println(classifier.toString());
            break;
        }
        send();
      }

/*
      // load packages
      // deal with command line options

      //BufferedReader in = new Buffnew DataInputStream(new FileInputStream(toWekaName));
      BufferedReader in = new BufferedReader(new FileReader(toWekaName));
          //= new BufferedReader(new InputStreamReader(in));
      PrintStream out = new PrintStream(new FileOutputStream(fromWekaName));
      
      // build an initial classifier
			classifier.buildClassifier(trainData);

      while (true) {
        line = in.readLine();
        if ((line == null) || line.equals("exit")) {
          break;
        } else if (line.equals("add")) {
          Instance inst = readInst(in,trainData.numAttributes());
          trainData.add(inst);
        } else if (line.equals("train")) {
          classifier.buildClassifier(trainData);
        } else if (line.equals("classify")) {
          Instance inst = readInst(in,trainData.numAttributes());
          testData.add(inst);
          double[] distr = classifier.distributionForInstance(testData.lastInstance());
          testData.delete();
          for (int i = 0; i < distr.length; i++) {
            out.format("%f ",distr[i]);
          }
          out.format("%n");
        } else if (line.equals("")) {
          continue;
        } else if (line.equals("save")) {
          line = in.readLine();
          System.out.println("SAVING " + line);
          saveModel(line);
        } else {
          System.err.println("ERROR: unexpected line:");
          System.err.println(line);
          break;
        }
      }

      
      in.close();
      out.close();
      return;
*/
    } catch (Exception e) {
      System.err.println("EXCEPTION:");
      System.err.println(e.toString());
      e.printStackTrace();
    }
  }
  public static Instance readInst(BufferedReader in, int numFeatures) throws java.io.IOException {
    double weight;
    double[] vals = new double[numFeatures];
    String line = in.readLine();
    String[] strVals = line.split(",");
    assert(strVals.length == numFeatures + 1);

    for (int i = 0; i < numFeatures; i++) {
      vals[i] = Double.parseDouble(strVals[i]);
    }
    weight = Double.parseDouble(strVals[numFeatures]);
    return new DenseInstance(weight,vals);
  }

	public static void saveModel(String filename) {
		try {
			weka.core.SerializationHelper.write(filename,classifier);
		} catch (Exception e) {
			System.out.println(e.getMessage());
		}
	}
}

