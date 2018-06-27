const Datastore = require('@google-cloud/datastore');
const projectId = 'masteruma04';
const datastore = new Datastore({ projectId: projectId });

exports.subscribe = (event, callback) => {
  const pubsubMessage = event.data;
  const kind = 'Vineyard2';
  const taskKey = datastore.key(kind);
  const task = {key: taskKey,
  data: { description: Buffer.from(pubsubMessage.data, 'base64').toString()}, };
  datastore.save(task).then(() => { console.log('Saved: ${task.data.description}'); })
  .catch(err => { console.error('ERROR:', err); });
  callback();
};