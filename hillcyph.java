/* Encrypts alphabetic letters in a file from size 2x2 to 9x9 program will
take in two command line paramters containing the names of the file storing
the encryption key & the file to be encrypted*/

import java.util.*;
import java.io.File;
import java.io.*;

public class hillcipher
{
	public static void main(String[] args) throws FileNotFoundException
	{
		
		//Get the file names from the command line and store its
		// name for future reference when scanning the file contents
		Scanner stdin = new Scanner(System.in);
		Scanner keyScan = null;
		Scanner fileScan = null;
		int count = 0; 
		
		try
		{
		keyScan = new Scanner(new File(args[0]));
		fileScan = new Scanner(new File(args[1]));		
		}
		
		catch(FileNotFoundException ex)
		{
			System.out.println("File not found, please enter a valid file name");
		}
		
		
		//Get the keysize of the array and scan its contents into an array
		int keySize = keyScan.nextInt(); 
		
		//Call the method that will store the values of the key array and print them
		ArrayList <Integer> key = hillcipher.keySaverPrinter(keySize,keyScan);
		
		
		//Call the function that will print your plaintext characters
		//& store them into an arrayList
		ArrayList <Character> plainText = hillcipher.plainTextPrinter(fileScan,keySize);
		System.out.print("\n\n");
		
		//Call the function that converts our values to ciphertext
		ArrayList<Character> cipherText = new ArrayList<Character>();
		
		System.out.println("\nCiphertext:\n");		
		
		//We are doing matrix multiplication working with letter blocks of size "keySize"
		// recall plainText is the arraylist that stores our plaintext chars
		for(int p= 0; p<plainText.size(); p+=keySize)
		{
			  hillcipher.cipherTextConv(p,plainText,cipherText,key,keySize);
		}
		
		System.out.println("\n");
	}
	
	//Function input: takes in the size of the key and the scanner for the key
	//Function output: Will print the key matrix and return its values in a 2d array
	public static ArrayList<Integer> keySaverPrinter(int size,Scanner keyScan)
	{
		int[][] key = new int [size][size];
		ArrayList <Integer> myKey = new ArrayList<Integer>();

		
		//We will print and store the key matrix in the following lines 
		System.out.println("\n\nKey matrix: \n");
		
		for(int i=0; i< size; i++)
		{
			for(int j=0; j<size;j++)
			{
				key[i][j] = keyScan.nextInt();
				myKey.add(key[i][j]);
				System.out.print(key[i][j]+ " ");
				
				if(j+1==size)
					System.out.print("\n");
			}
		}
		
		//print an extra line for the plaintext output
		System.out.print("\n");

		//Keep it neat! B/c we no longer need the key file, close it.
		keyScan.close();
		
		return myKey;
	}
	
	//Function input: function takes in the scanner that scans the message to be encrypted
	//Function output: function returns a 2d array storing all of the letters in lower case
	public static ArrayList<Character> plainTextPrinter(Scanner messageScanner,int keySize)
	{
		ArrayList <Character> plainText = new ArrayList<Character>();
		System.out.println("\nPlaintext: \n");
		
		//Read the file line by line
		while(messageScanner.hasNextLine())
		{
			//Scan in the line with spaces and all 	converting uppercase letter to lowercase
			String inputLine = messageScanner.nextLine().replaceAll("[^a-zA-Z]","").toLowerCase();
			
			//Create a new string sans white spaces 
			String actualLine = inputLine.replaceAll("\\s","");
			
			//Traverse over the current line storing the letter characters into an array
			for(int j=0;j<actualLine.length();j++)
			{	
				char c = actualLine.charAt(j);
				
				//We will only add the char to the arrayList if it is a letter
				if(Character.isLetter(c))
				{		
					plainText.add(c);
					System.out.print(c);
					
					//if we have reached 80 chars start printing the characters on a new line
					if((plainText.size()%80==0))
					{
						System.out.print("\n");
					}
				}
			
			}
		}
		
		//Keep it clean!!
		messageScanner.close();
		
		//If padding is needed 
		if(!((plainText.size()%keySize)==0))
		{
			//The number of times we will be printing x
			int addChar = keySize - (plainText.size()%keySize);
			
			for(int p =0;p<addChar;p++)
			{
				plainText.add('x');
				
				if((plainText.size()%80==0))
					{
						System.out.print("\n");
					}
				System.out.print("x");
			}
		}
		
		return plainText;
	}
	
	//Function input: Function takes in the inedex value of the plaintext array we will continuously iterate over in "keysize" increments
	//				function will also be manipulating a ciphertext array list, and of course we need an arraylist that stores our key
	//Function output: returns nothing, but it does make a sum of the rows and passes that value to the function that convers the sum to ciphertext
	//					letters.
	public static void cipherTextConv(int p, ArrayList<Character> plainText,  ArrayList<Character> cipherText, ArrayList<Integer> key, int keySize)
	{
			int sum = 0;
			int i =0;
			int start = p;
			int count = 0;
			
			//Iterate over each value stored in our key
			while(i<=key.size())
			{
				//if we have added values to sum "keysize" times then it is time to print a ciphertext value
				//to the screen!
				if(((count)%keySize==0)&&count!=0)
				{
					hillcipher.printMyCipherText(sum,cipherText);
					
					//P goes back to it's intial value because we are only working with this block of keysize letters
					p=start;
					
					//Reset sum to have a valid value for the next row
					sum =0;
				}
				
				//This means we are out of bounds....so let's not let this happen
				if(i==key.size())
				{
					break;
				}
				
				//Do matrix multiplication !
				sum+= (int)(((plainText.get(p)) - 'a') * (key.get(i)));
				p++;
				i++;
				count++;
			}
			
							
	}
	
	//Function input: Function takes in the sum of the row and the ciphertext char arraylist
	//Function output: function will convert the sum (block of "keysize" leters) to ciphertext & print 
	//					the letter to the screen
	public static void printMyCipherText(int sum,ArrayList<Character>cipherText)
	{
		//Convert the block sum to a char 
		char letter = (char) ((sum %26 )+'a');
		
		//add it to a arraylist of characters to keep track of how many letters are on the line
		cipherText.add(letter);
		
		System.out.print(letter);
		
		//If we have reached 80 characters then we start printing on a new line
		if((cipherText.size()%80)==0)
		{
			System.out.print("\n");
		}
		
		
	}
	
	
}
