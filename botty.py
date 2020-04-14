

import discord
import pyrebase
import datetime
from dotenv import load_dotenv

#setup environment variables
load_dotenv()

#discord auth token
token = "discord token"

#firebase auth variables
config = {
    "apiKey": "apikey",
    "authDomain": "discord-cloak.firebaseapp.com",
    "databaseURL": "https://discord-cloak.firebaseio.com",
    "storageBucket": "discord-cloak.appspot.com"
}

#which users are being tracked
trackedusers = {"green": 0, "blue": 0, "red": 2, "yellow": 0};

#whether the bot is on or not
on = True

#set up the firebase environment
firebase = pyrebase.initialize_app(config)

#get the database
db = firebase.database()

#find the tracked user from database and store it locally
users = db.child("users").get()
trackedusers = users.val()

#intialize discord
client = discord.Client()

#when the bot connects
@client.event
async def on_ready():

	#print some stuff to confirm it's connected
    guild = discord.utils.get(client.guilds, name="Yuuko's Mad Ramblings")
    print(
        f'{client.user} is connected to the following guild:\n'
        f'{guild.name}(id: {guild.id})'
    )

#On a message
@client.event
async def on_message(message):
    global on

	#if the bot was the one who sent it, ignore
    if message.author == client.user:
        return

	#split the message on ":"
	#If it finds one of the messages of the form
	#c:text
	#d:text
	#l:text
	#then it's a pluralkit command, so ignore
    check = message.content.split(":")
    if check[0] == "c":
        pass
    elif check[0] == "d":
        pass
    elif check[0] == "l":
        pass


    else:

		#split the message on spaces
        messagebits = message.content.split(" ")

		#t!color adds users to tracking based on color
        if messagebits[0] == 't!color':

			#if the message has atleast two parts
            if len(messagebits) >=2:

				#first bit is the color
                color = messagebits[1]
                if color in trackedusers.keys():

					#If it has three parts
                    if len(messagebits) == 3:

						#the three part can be an @user, so find the user'sname and save them to tracked users
                        if "@" in messagebits[2]:
                            id = int(messagebits[2].lstrip("<@!").rstrip(">"))
                            print(message.guild.get_member(id).name)
                            trackedusers[color] = message.guild.get_member(id).name
                            db.child("users").update(trackedusers)

							#Send confirmation
                            await message.channel.send("Understood, "+message.guild.get_member(id).name+" will be " + color)

						#otherwise it's just a name, so save that
                        else:
                            name = messagebits[2]
                            trackedusers[color] = name
                            db.child("users").update(trackedusers)

							#send confirmation
                            await message.channel.send("Understood, "+name+" will be " + color)

					#otherwise it's just two parts, so the send must want to be assigned, so save them
                    elif len(messagebits) == 2:
                        id = message.author.name
                        trackedusers[color] = id
                        db.child("users").update(trackedusers)

						#send confirmation
                        await message.channel.send("yes you will be " + color)

		#If the user asked t!ready, reply with ready
        elif messagebits[0] == 't!ready':
            await message.channel.send("Ready! :^)")

		#t!rebuild repulls the user database to trackedusers
        elif messagebits[0] == 't!rebuild':
            update = db.child("users").get().val()
            for key in update.keys():
                trackedusers[key] = update[key]

		#t!users simply replies with the users in trackedusers
        elif messagebits[0] == "t!users":
            for key in trackedusers:
                await message.channel.send(key + " : " + trackedusers[key])

		#t!clear clears the database of all messages
        elif messagebits[0] == "t!clear":
            db.child("messages").remove()
            db.child("messages").push(
                {"user": "green", "count": 0})
            await message.channel.send("db has been clear!")

		#t!on turns the tracking on
        elif messagebits[0] == "t!on":
            on = True
            await message.channel.send("Discozy is on!")

		#t!off turns the tracking off
        elif messagebits[0] == "t!off":
            on = False
            await message.channel.send("Discozy is off :(")

		#if the system is not on, don't track messages
        elif not on:
            pass

		#on any other message
        else:
			#If it's from a tracked member
            for key in trackedusers.keys():
                if message.author.name == trackedusers[key]:

					#add their message to the database
                    db.child("messages").push({"user": key, "count": len(message.content.split(" "))})



client.run(token)
