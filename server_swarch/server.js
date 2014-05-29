// We set up a datagram to send data between the webserver and the game server
// The FileSystem is a type of file reader/writer that allows us to save and update high scores
var dgram = require('dgram'),
fileSystem = require('fs'),
highScores,
server;

// Load high scores from file
fileSystem.readFile("HighScores.txt", 'utf8', function(err,data){
	
	// If an error occurred loading our file, print an error message and exit
	if(err)
	{
		console.log("An error occurred loading our High Scores");
		process.exit();
	}
	
	// Begin loading the high scores from our file
	console.log("Loading high scores from file...");
	 
	try{
		// Use JSON to turn file contents back into a Javascript Array Object
		highScores = JSON.parse(data);	
	}catch(e){
		// Exception occurred parsing the file
		console.log("Exception occurred parsing data");
		process.exit();
	}

	// Sort the high scores
	highScores.Scores.sort(function(a, b){
		return b.Score - a.Score;
	});
	
	// Display the sorted high scores
	console.log(highScores);
});

// Connect to the internet by UDP
server = dgram.createSocket('udp4');
console.log("Socket created");

// Add a handler for incoming traffic
server.on("message", function(msg, rinfo){
	console.log(rinfo);
	
		//SFML sends two packets, the first having the size of the next packet
		//We don't need it, so ignore anything that isn't a message we are trying to send
		if(rinfo.size != 4)
		{
			console.log("Received message: " + msg.toString());
			
				// Socket data comes in as a JSON Data Object
				var jsonData,i;
				
				try{
					// Parse the data received from the game server
					jsonData = JSON.parse(msg);
				}catch(exception){
					// The packet contained invalid JSON values, so exit
					console.log("Invalid JSON request received");
					return;
				}
			
			// Retrieve the action contained in the packet of data
			switch(jsonData.action)
			{
				// Add a score into our text file
				case "AddScore":
				console.log("AddScore called\n");
				
				// Make sure the high scores have been initialized
				if(highScores != undefined)
				{
					// Loop through the high scores and add the score
					for(i = 0; i < highScores.Scores.length; ++i)
					{
						// If the high score we are trying to add is bigger than the current entry
						if(highScores.Scores[i].Score < jsonData.score)
						{
							// Add the entry at the current point
							highScores.Scores.splice(i,0,{"Name" : jsonData.name, "Score" : jsonData.score});
							console.log("Inserted highscore by: " + jsonData.name);
							break;
						}
					}
				
					// Display the new high scores
					console.log(highScores.Scores);
				}
				break;
				
				// If a player obtained a better score, update their score
				case "UpdateScore":
				console.log("UpdateScore called");
				
				var userName = JSON.stringify(jsonData.name);
				
				// Check to make sure the highScores list isn't undefined
				if(highScores != undefined)
				{
					for(i = 0; i < highScores.Scores.length; ++i)
					{
						if(jsonData.name == highScores.Scores[i].Name)
						{
							// If the high score sent from the game server is better than the score currently on the web server, update it
							if(jsonData.score > highScores.Scores[i].Score)
							{
								highScores.Scores[i].Score = jsonData.score;
							}
						}
					}
				}
				
				break;
				
				// Once the game server has finished sending data to the web server, save the scores to a text file
				case "SaveScores":
				console.log("SaveScores called");
				
				if(highScores != undefined)
				{
					// Turn highscores back into JSON encoded string
					var highScoreAsString = JSON.stringify(highScores);
				
					// Print out the string that will be saved
					console.log(highScoreAsString + "\n\n");
				
					// Display the new high scores
					console.log(highScores.Scores);
				
					// Write the file to the given field
					fileSystem.writeFile('HighScores.txt', highScoreAsString, function (err) {
						if (err) 
							throw err;
						console.log("File saved!");
					});
				}
				break;
			}
		}
});

server.on("listening", function(){
	var address = server.address();
	console.log("server listening " +
        address.address + ":" + address.port);
});

// Bind the port
server.bind(1000);

// Make the website to post our high scores
var webServer = require('http');
webServer.createServer(function(req, res){

	res.writeHead(200, {'Content-Type': 'text/html'});
	res.write("<html><body><h1>High Score</h1><ul>");
	for(i=0; i< highScores.Scores.length; ++i)
	{
		res.write(highScores.Scores[i].Name + "&nbsp;&nbsp;" + highScores.Scores[i].Score + "<br />");
	}
	res.write("</ul></body></html>");
	res.end();

}).listen(80);