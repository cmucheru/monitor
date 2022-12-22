# monitoring-subsytem


## Steps to run web app

1. In the client directory, run:

```
npm install
```

installs the necessary dependencies for the client app

2. In the node -server directory, run:

```
```

installs the necessary dependencies for the server app

## How to run in production environment

In the node-server directory, run:

```
npm run prod
```

## How to run in development environment

In the node-server directory, you can run:

```
npm run dev
```

Runs the app in the development mode.\
Open [http://localhost:3000](http://localhost:3000) to view it in your browser.

## Running with custom environment variables on client

1. In the client directory create a .env.local file

```
touch .env.local
```

2. Add the following values to the .env.local file

```
REACT_APP_SERVER_URL=ws://<server hostname or ip address>:<server port> or http://<server hostname or ip address>:<server port>
```

## Running with custom environment variables on server

1. In the node-server directory create a .env file

```
touch .env
```

2. Add the following values to the .env file

```
BROKER_URL=mqtt://<broker hostname or ip address>:<broker port>
PORT=<server port>
ORIGIN=http://<client hostname or ip address>:<client port>
```

