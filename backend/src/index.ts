import { createApp } from './app';

const port = process.env.PORT ? Number(process.env.PORT) : 3000;
const app = createApp();

app.listen(port, () => {
  // eslint-disable-next-line no-console
  console.log(`Server listening on http://localhost:${port}`);
});